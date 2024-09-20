#include "config.h"

#include "config_map.h"

#include <libtrx/config/config_file.h>
#include <libtrx/filesystem.h>

struct json_object_s;

CONFIG g_Config = { 0 };

#define DIR_CONFIG "cfg"

static const char *m_ConfigPath = DIR_CONFIG "/TR2X.json5";

static void M_Load(struct json_object_s *root_obj);
static void M_Dump(struct json_object_s *root_obj);

static void M_Load(struct json_object_s *root_obj)
{
    ConfigFile_LoadOptions(root_obj, g_ConfigOptionMap);
}

static void M_Dump(struct json_object_s *root_obj)
{
    ConfigFile_DumpOptions(root_obj, g_ConfigOptionMap);
}

bool Config_Read(void)
{
    return ConfigFile_Read(m_ConfigPath, &M_Load);
}

bool Config_Write(void)
{
    File_CreateDirectory(DIR_CONFIG);
    return ConfigFile_Write(m_ConfigPath, &M_Dump);
}

void Config_Sanitize(void)
{
}

void Config_ApplyChanges(void)
{
}

const CONFIG_OPTION *Config_GetOptionMap(void)
{
    return g_ConfigOptionMap;
}
