#ifndef CTRE__ATOMS_CHARACTERS__HPP
#define CTRE__ATOMS_CHARACTERS__HPP

#include "utility.hpp"
#include "flags_and_modes.hpp"

#ifndef CTRE_IN_A_MODULE
#include <cstdint>
#endif

namespace ctre {
	
// sfinae check for types here

template <typename T> class MatchesCharacter {
	template <typename Y, typename CharT> static auto test(CharT c) -> decltype(Y::match_char(c, std::declval<const flags &>()), std::true_type());
	template <typename> static auto test(...) -> std::false_type;
public:
	template <typename CharT> static inline constexpr bool value = decltype(test<T>(std::declval<CharT>()))();
};

template <typename T> constexpr CTRE_FORCE_INLINE bool is_ascii_alpha(T v) {
	return ((v >= static_cast<T>('a') && v <= static_cast<T>('z')) || (v >= static_cast<T>('A') && v <= static_cast<T>('Z')));
}

template <typename T> constexpr CTRE_FORCE_INLINE bool is_ascii_alpha_lowercase(T v) {
	return (v >= static_cast<T>('a')) && (v <= static_cast<T>('z'));
}

template <typename T> constexpr CTRE_FORCE_INLINE bool is_ascii_alpha_uppercase(T v) {
	return (v >= static_cast<T>('A')) && v <= (static_cast<T>('Z'));
}

template <auto V> struct character {
	template <typename CharT> CTRE_FORCE_INLINE static constexpr bool match_char(CharT value, const flags & f) noexcept {
		if constexpr (is_ascii_alpha(V)) {
			if (is_case_insensitive(f)) {
				if (value == (V ^ static_cast<decltype(V)>(0x20))) {
					return true;//
				}
			}	
		}
		return value == V;
	}
};

template <typename... Content> struct negative_set {
	template <typename CharT> CTRE_FORCE_INLINE static constexpr bool match_char([[maybe_unused]] CharT value, const flags & f) noexcept {
		return !(Content::match_char(value, f) || ... || false);
	}
};

template <typename... Content> struct set {
	template <typename CharT> CTRE_FORCE_INLINE static constexpr bool match_char([[maybe_unused]]  CharT value, const flags & f) noexcept {
		return (Content::match_char(value, f) || ... || false);
	}
};

template <auto... Cs> struct enumeration : set<character<Cs>...> { };

template <typename... Content> struct negate {
	template <typename CharT> CTRE_FORCE_INLINE static constexpr bool match_char([[maybe_unused]] CharT value, const flags & f) noexcept {
		return !(Content::match_char(value, f) || ... || false);
	}
};

template <auto A, auto B> struct char_range {
	template <typename CharT> CTRE_FORCE_INLINE static constexpr bool match_char(CharT value, const flags & f) noexcept {
		if constexpr (is_ascii_alpha_lowercase(A) && is_ascii_alpha_lowercase(B)) {
			if (is_case_insensitive(f)) {
				if (value >= (A ^ static_cast<decltype(A)>(0x20)) && value <= (B ^ static_cast<decltype(B)>(0x20))) {
					return true;//
				}
			}	
		} else if constexpr (is_ascii_alpha_uppercase(A) && is_ascii_alpha_uppercase(B)) {
			if (is_case_insensitive(f)) {
				if (value >= (A ^ static_cast<decltype(A)>(0x20)) && value <= (B ^ static_cast<decltype(B)>(0x20))) {
					return true;//
				}
			}	
		}
		return (value >= A) && (value <= B);
	}
};
using word_chars = set<char_range<'A','Z'>, char_range<'a','z'>, char_range<'0','9'>, character<'_'> >;

using space_chars = enumeration<' ', '\t', '\n', '\v', '\f', '\r'>;

using vertical_space_chars = enumeration<
	char{0x000A}, // Linefeed (LF)
	char{0x000B}, // Vertical tab (VT)
	char{0x000C}, // Form feed (FF)
	char{0x000D}, // Carriage return (CR)
	char32_t{0x0085}, // Next line (NEL)
	char32_t{0x2028}, // Line separator
	char32_t{0x2029} // Paragraph separator
>;

using horizontal_space_chars = enumeration<
    char{0x0009}, // Horizontal tab (HT)
    char{0x0020}, // Space
    char32_t{0x00A0}, // Non-break space
    char32_t{0x1680}, // Ogham space mark
    char32_t{0x180E}, // Mongolian vowel separator
    char32_t{0x2000}, // En quad
    char32_t{0x2001}, // Em quad
    char32_t{0x2002}, // En space
    char32_t{0x2003}, // Em space
    char32_t{0x2004}, // Three-per-em space
    char32_t{0x2005}, // Four-per-em space
    char32_t{0x2006}, // Six-per-em space
    char32_t{0x2007}, // Figure space
    char32_t{0x2008}, // Punctuation space
    char32_t{0x2009}, // Thin space
    char32_t{0x200A}, // Hair space
    char32_t{0x202F}, // Narrow no-break space
    char32_t{0x205F}, // Medium mathematical space
    char32_t{0x3000} // Ideographic space
>;

using alphanum_chars = set<char_range<'A','Z'>, char_range<'a','z'>, char_range<'0','9'> >;

using alpha_chars = set<char_range<'A','Z'>, char_range<'a','z'> >;

using xdigit_chars = set<char_range<'A','F'>, char_range<'a','f'>, char_range<'0','9'> >;

using punct_chars
    = enumeration<'!', '"', '#', '$', '%', '&', '\'', '(', ')', '*', '+', ',', '-',
		  '.', '/', ':', ';', '<', '=', '>', '?', '@', '[', '\\', ']',
		  '^', '_', '`', '{', '|', '}', '~'>;

using digit_chars = char_range<'0','9'>;

using ascii_chars = char_range<'\x00','\x7F'>;


}

#endif
