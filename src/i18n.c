#include "i18n.h"
#include "i18n-helper.h"
#include "options.h"
#include "log.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef GDI
  #include <windows.h>
  #ifndef LCIDToLocaleName
    WINBASEAPI int WINAPI LCIDToLocaleName(LCID Locale, LPWSTR lpName, int cchName, DWORD dwFlags);
  #endif
#endif

// FIXME: You must follow the order of langs[] in i18n_map[][]
static lang_map langs[] = {
  "en_US",
  "zh_CN",
  "zh_TW",
  "zh_HK",
  "ru_RU",
  "ja_JP",
  "it_IT",
  "fr_FR",
  "nl_NL",
};

// selected language id
static size_t ilang = -1;

// FIXME: You must follow the order of langs[] here
#define platform_preset(platform) \
  { \
    platform_i18n(platform, "en_US", "Activate ",, "Go to Settings to activate ", ".") \
    platform_i18n(platform, "zh_CN", "激活 ",, "转到“设置”以激活 ", "。") \
    platform_i18n(platform, "zh_TW", "啟用 ",, "移至[設定]以啟用 ", "。") \
    platform_i18n(platform, "zh_HK", "啟用 ",, "移至[設定]以啟用 ", "。") \
    platform_i18n(platform, "ru_RU", "Активация ",, "Чтобы активировать ", ",\nперейдите в раздел \"Параметры\".") \
    platform_i18n(platform, "ja_JP",,"のライセンス認証", "設定を開き、", "のライセンス認証を行ってください") \
    platform_i18n(platform, "it_IT", "Attiva ",, "Passa a Impostazioni per attivare ", ".") \
    platform_i18n(platform, "fr_FR", "Activer ",, "Aller en les paramètres pour activer ", ".") \
    platform_i18n(platform, "nl_NL", "Activeren ",, "Gaan naar instellingen om te activeren ", ".")\
  },

// FIXME: You must follow the order of langs[] here
#define ms_diss_preset() \
  { \
    system_i18n ("en_US", "No need to activate ",, "We're not as annoying as Microsoft.") \
    untranslated("zh_CN") \
    untranslated("zh_TW") \
    untranslated("zh_HK") \
    system_i18n ("ru_RU", "Не требуется активировать ",, "Мы не настолько назойливы, как Microsoft.") \
    untranslated("ja_JP") \
    untranslated("it_IT") \
    untranslated("fr_FR") \
    untranslated("nl_NL") \
  },

// FIXME: You must follow the order of presets[] in i18n_map[]
static preset_map presets[] = {
  map_preset("mac",       "Platform preset for macOS")
  map_preset("bsd",       "Platform preset for *BSD")
  map_preset("linux",     "Platform preset for Linux")
  map_preset("hurd",      "Platform preset for GNU/Hurd")
  map_preset("windows",   "Platform preset for Windows")
  map_preset("unix",      "Platform preset for *nix")
  map_preset("m$",        "Diss M!cr0$0f+")
  map_preset("deck",      "Platform preset for the Steam Deck")
  map_preset("reactos",   "Platform preset for ReactOS")
};

// FIXME: You must follow the order of presets[] here
i18n_info i18n_map[][length(langs)] = {
  platform_preset("macOS")      // macOS platform preset
  platform_preset("BSD")        // BSD platform preset
  platform_preset("Linux")      // Linux platform preset
  platform_preset("GNU/Hurd")   // GNU/Hurd platform preset
  platform_preset("Windows")    // Windows platform preset
  platform_preset("*nix")       // *nix platform preset
  ms_diss_preset()              // ms-diss system preset
  platform_preset("Steam Deck") // Steam Deck platform preset
  platform_preset("ReactOS")    // ReactOS platform preset
};

void i18n_set_info(const char *const preset) {
  size_t ipreset = DEFAULT_PRESET;

  if (ilang == (size_t)-1) {
    ilang = length(langs);
#ifdef GDI
    // LCIDToLocaleName is available starting from Vista
    // if you want to compile activate-linux for XP and ReactOS
    // please, do one of:
    // * compile with https://github.com/Chuyu-Team/VC-LTL/blob/master/src/ucrt/locale/lcidtoname_downlevel.cpp
    // * replace code below with: char lang[] = "ru_RU";
    #define LANG_STR_SIZE 6
    wchar_t langw[LANG_STR_SIZE];
    LCIDToLocaleName(LOCALE_USER_DEFAULT, langw, LANG_STR_SIZE, 0);
    char lang[LANG_STR_SIZE];
    for (int i=0; i<LANG_STR_SIZE; i++) lang[i] = langw[i];
    lang[2] = '_';
    #undef LANG_STR_SIZE
#else
    char *lang = getenv("LANG");
#endif
    while(--ilang && ! match_str(langs[ilang], lang));
  }

  if (preset != NULL) {
    ipreset = length(presets);
    while (--ipreset && ! match_str(presets[ipreset].name, preset));
    if (!match_str(presets[ipreset].name, preset)) {
      __warn__("Undefined preset: %s\n", preset);
      ipreset = DEFAULT_PRESET;
    }
  }

  __info__("Loading preset: %s\n", presets[ipreset].name);
  if (!(PRESET.title && PRESET.subtitle))
    ilang = 0;

  options.title = PRESET.title;
  options.subtitle = PRESET.subtitle;
}

#define HELP(fmtstr, ...) fprintf(stderr, "  " fmtstr "\n", ## __VA_ARGS__)
#define STYLE(x) "\033[" # x "m"
#define END() fprintf(stderr, "\n")
void i18n_list_presets() {
  fprintf(stderr, "Built-in Presets:\n\n");

  HELP(STYLE(1) "Name\t\tDescription" STYLE(0));
  for (size_t len = 0; len < length(presets); len++)
    HELP(STYLE(1) "%s"STYLE(0) "\t\t%s", presets[len].name, presets[len].info);
}
#undef END
#undef STYLE
#undef HELP
