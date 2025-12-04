#include <doctest/doctest.h>
#include "dscore/LanguageService.h"

TEST_SUITE("LanguageService") {
    TEST_CASE("Language Switching") {
        sss::dscore::LanguageService language_service;

        SUBCASE("Default locale is system default or set in constructor") {
            // Just check it returns something valid
            CHECK(language_service.GetCurrentLocale() != QLocale::c()); 
        }

        SUBCASE("SwitchLanguage updates current locale") {
            QLocale new_locale(QLocale::German);
            language_service.SwitchLanguage(new_locale);
            CHECK(language_service.GetCurrentLocale() == new_locale);

            QLocale another_locale(QLocale::French);
            language_service.SwitchLanguage(another_locale);
            CHECK(language_service.GetCurrentLocale() == another_locale);
        }
    }

    TEST_CASE("Translator Registration") {
        sss::dscore::LanguageService language_service;

        SUBCASE("RegisterTranslator accepts inputs") {
            // We cannot easily verify internal state without friend classes or public getters,
            // but we can ensure it doesn't crash.
            language_service.RegisterTranslator("TestComponent", "/path/to/translations");
            
            // Trigger a switch to see if it handles the registered path (even if invalid) gracefully
            language_service.SwitchLanguage(QLocale::English);
        }
    }
}
