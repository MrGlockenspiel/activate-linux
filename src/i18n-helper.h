#ifndef __I18N_HELPER__
#define __I18N_HELPER__

#ifdef __APPLE__
#define DEFAULT_PRESET 0
#elif __FreeBSD__
#define DEFAULT_PRESET 1
#else
#define DEFAULT_PRESET 2
#endif

// typedefs
typedef struct {
	char *name, *info;
} preset_map ;

typedef char* lang_map;

#define match_str(match, with) strncmp(match, with, 5) == 0
#define length(array) (sizeof(array) / sizeof(array[0]))
#define map_preset(name, description) {name, description},
#define _LANG(id, country) id "_" country,

// preset generator macro
#define untranslated(lang)  {NULL, NULL},
#define system_i18n(lang, prefix, suffix, subtitle) {prefix SYSTEM_NAME suffix, subtitle},
#define platform_i18n(name, lang, prefix, suffix, presub, sufsub) \
						{prefix name suffix, presub name sufsub},

#define PRESET i18n_map[ipreset][ilang]

#endif
