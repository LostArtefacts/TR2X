CWD := `pwd`
HOST_USER_UID := `id -u`
HOST_USER_GID := `id -g`

default: (build-win "debug")

_docker_push tag:
    docker push {{tag}}

_docker_build dockerfile tag force="0":
    #!/usr/bin/env sh
    if [ "{{force}}" = "0" ]; then
        docker images --format '{''{.Repository}}' | grep '^{{tag}}$'
        if [ $? -eq 0 ]; then
            echo "Docker image {{tag}} found"
            exit 0
        fi
        echo "Docker image {{tag}} not found, trying to download from DockerHub"
        if docker pull {{tag}}; then
            echo "Docker image {{tag}} downloaded from DockerHub"
            exit 0
        fi
        echo "Docker image {{tag}} not found, trying to build"
    fi

    echo "Building Docker image: {{dockerfile}} → {{tag}}"
    docker build \
        --progress plain \
        . \
        -f {{dockerfile}} \
        -t {{tag}}

_docker_run *args:
    @echo "Running docker image: {{args}}"
    docker run \
        --rm \
        --user \
        {{HOST_USER_UID}}:{{HOST_USER_GID}} \
        --network host \
        -v {{CWD}}:/app/ \
        {{args}}


image-win force="1":              (_docker_build "tools/docker/game-win/Dockerfile" "rrdash/tr2x" force)
image-win-config force="1":       (_docker_build "tools/docker/config/Dockerfile" "rrdash/tr2x-config" force)

push-image-win:                   (image-win "0") (_docker_push "rrdash/tr2x")

build-win target='debug':         (image-win "0")        (_docker_run "-e" "TARGET="+target "rrdash/tr2x")
build-win-config:                 (image-win-config "0") (_docker_run "rrdash/tr2x-config")

package-win target='release':     (build-win target) (_docker_run "rrdash/tr2x" "package")
package-win-all target='release': (build-win target) (build-win-config) (_docker_run "rrdash/tr2x" "package")

output-current-version:
    tools/get_version

output-current-changelog:
    tools/output_current_changelog

clean:
    -find build/ -type f -delete
    -find tools/ -type f \( -ipath '*/out/*' -or -ipath '*/bin/*' -or -ipath '*/obj/*' \) -delete
    -find . -mindepth 1 -empty -type d -delete

lint-imports:
    tools/sort_imports

lint-format:
    pre-commit run -a

lint: (lint-imports) (lint-format)
