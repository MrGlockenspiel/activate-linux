#ifndef __I18N_HELPER__
#define __I18N_HELPER__

#if defined(__APPLE__) || defined(__MACH__)
#define DEFAULT_PRESET 0
#define SYSTEM_NAME "macOS"

#elif defined(__FreeBSD__) || defined(__NetBSD__) \
		|| defined(__OpenBSD__) || defined(__DragonFly__) \
		|| defined(__bsdi__)
#define DEFAULT_PRESET 1
#define SYSTEM_NAME "*BSD"

#elif defined(__linux__)
#define DEFAULT_PRESET 2
#define SYSTEM_NAME "Linux"

#elif defined(__gnu_hurd__) || defined(__GNU__)
#define DEFAULT_PRESET 3
#define SYSTEM_NAME "GNU/Hurd"

#elif defined(_WIN32) || defined(_WIN64) || defined(__MSYS__)
#define DEFAULT_PRESET 4
#define SYSTEM_NAME "Windows"

#elif defined(__unix__)
#define DEFAULT_PRESET 5
#define SYSTEM_NAME "*nix"

#endif

// typedefs
typedef struct {
	char *name, *info;
} preset_map ;

typedef char *lang_map;

/****************************************************************
 * match_str(match, with)
 *
 * Check whether @match matches @with
 * @note  both @match and @with can be at most 5
 *
 * @param match Match this param
 * @param with  Match with this param
 * @return true if both arg matches
 ****************************************************************/
#define match_str(match, with) strncmp(match, with, 5) == 0

/****************************************************************
 * length(array)
 *
 * Return length of @array
 *
 * @param array The array to get length of
 * @return length of @array
 ****************************************************************/
#define length(array) (sizeof(array) / sizeof(array[0]))

/****************************************************************
 * map_preset(name, description)
 *
 * Define a preset
 *
 * @param name        Preset name
 * @param description Preset Description
 * @return preset struct
 ****************************************************************/
#define map_preset(name, description) {name, description},

/****************************************************************
 * untranslated(lang)
 *
 * Define untranslated preset
 *
 * @param lang    Target language
 * @return i18n_info{NULL, NULL}
 ****************************************************************/
#define untranslated(lang)  {NULL, NULL},

/****************************************************************
 * system_i18n(lang, prefix, suffix, subtitle)
 *
 * Define a preset using SYSTEM_NAME
 *
 * @param lang     Target language
 * @param prefix   Title prefix
 * @param suffix   Title suffix
 * @param subtitle Watermark subtitle
 ****************************************************************/
#define system_i18n(lang, prefix, suffix, subtitle) {prefix SYSTEM_NAME suffix, subtitle},

/****************************************************************
 * platform_i18n(name, lang, prefix, suffix, presub, sufsub)
 *
 * Define a platform specific preset
 *
 * @param name    Target platform
 * @param lang    Target language
 * @param prefix  Title prefix
 * @param suffix  Title suffix
 * @param presub  Subtitle prefix
 * @param sufsub  Subtitle suffix
 ****************************************************************/
#define platform_i18n(name, lang, prefix, suffix, presub, sufsub) \
						{prefix name suffix, presub name sufsub},

#define PRESET i18n_map[ipreset][ilang]

#endif
