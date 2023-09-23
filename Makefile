CWD = $(shell pwd)
HOST_USER_UID = $(shell id -u)
HOST_USER_GID = $(shell id -g)

define build
	$(eval TARGET := $(1))
	mkdir -p build
	docker run --rm \
		--user $(HOST_USER_UID):$(HOST_USER_GID) \
		--entrypoint /app/docker/game-win/entrypoint.sh \
		-e TARGET="$(TARGET)" \
		-v $(CWD):/app/ \
		rrdash/tr2x:latest
endef

build-docker-image:
	docker build --progress plain . -f docker/game-win/Dockerfile -t rrdash/tr2x

debug:
	$(call build,debug)

debugopt:
	$(call build,debugoptimized)

release:
	$(call build,release)

clean:
	-find build/ -type f -delete
	-find build/ -mindepth 1 -empty -type d -delete

imports:
	tools/sort_imports

lint:
	bash -c 'shopt -s globstar; clang-format -i **/*.c **/*.h'

.PHONY: debug debugopt release clean imports lint

