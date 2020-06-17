#pragma once
#include <typeinfo>
#include <gsl/gsl_assert>
#include <algorithm>
#include "halleystring.h"
#include <halley/support/exception.h>

namespace Halley
{

	template <typename T>
	struct EnumNames {
	};


	
	struct UserConverter
	{
		template<typename T, typename std::enable_if<std::is_enum<T>::value, int>::type = 0>
		static String toString(const T& v)
		{
			return EnumNames<T>()()[int(v)];
		}

		template<typename T, typename std::enable_if<!std::is_enum<T>::value, int>::type = 0>
		static String toString(const T& v)
		{
			return v.toString();
		}

		template<typename T, typename std::enable_if<std::is_enum<T>::value, int>::type = 0>
		static T fromString(const String& str)
		{
			EnumNames<T> n;
			auto names = n();
			auto res = std::find_if(std::begin(names), std::end(names), [&](const char* v) { return str == v; });
			if (res == std::end(names)) {
				throw Exception("String \"" + str + "\" does not exist in enum \"" + typeid(T).name() + "\".", HalleyExceptions::Utils);
			}
			return T(res - std::begin(names));
		}

		template<typename T, typename std::enable_if<!std::is_enum<T>::value, int>::type = 0>
		static T fromString(const String& str)
		{
			return T(str);
		}
	};


	template <typename T>
	struct ToStringConverter {
		String operator()(const T& s) const
		{
			return UserConverter::toString(s);
		}
	};

	template <typename T>
	struct FromStringConverter {
		T operator()(const String& s) const
		{
			return UserConverter::fromString<T>(s);
		}
	};

	template<size_t N>
	struct ToStringConverter<char[N]>
	{
		String operator()(const char s[N]) const
		{
			return String(s);
		}
	};
	
	template<>
	struct ToStringConverter<const char*>
	{
		String operator()(const char* s) const
		{
			return String(s);
		}
	};

	template<>
	struct ToStringConverter<char*>
	{
		String operator()(const char* s) const
		{
			return String(s);
		}
	};

	template<size_t N>
	struct ToStringConverter<wchar_t[N]>
	{
		String operator()(const wchar_t s[N]) const
		{
			return String(s);
		}
	};

	template<>
	struct ToStringConverter<const wchar_t*>
	{
		String operator()(const wchar_t* s) const
		{
			return String(s);
		}
	};

	template<>
	struct ToStringConverter<wchar_t*>
	{
		String operator()(const wchar_t* s) const
		{
			return String(s);
		}
	};

	template<>
	struct ToStringConverter<std::string>
	{
		String operator()(const std::string& s) const
		{
			return String(s);
		}
	};

	template<>
	struct ToStringConverter<bool>
	{
		String operator()(bool s) const
		{
			return String(s ? "true" : "false");
		}
	};

	template<>
	struct FromStringConverter<bool>
	{
		bool operator()(const String& s) const
		{
			return s == "true";
		}
	};

	
	template <typename T, typename std::enable_if<std::is_floating_point<T>::value, int>::type = 0>
	String toString(T src, int precisionDigits = -1, char decimalSeparator = '.')
	{
		Expects(precisionDigits >= -1 && precisionDigits <= 20);
		std::stringstream str;
		if (precisionDigits != -1) {
			str << std::fixed << std::setprecision(precisionDigits);
		}
		str << src;

		String result;
		if (precisionDigits == -1) {
			result = String::prettyFloat(str.str());
		} else {
			result = str.str();
		}

		if (decimalSeparator != '.') {
			result = result.replaceAll(String("."), String(decimalSeparator));
		}

		return result;
	}

	template <typename T, typename std::enable_if<std::is_integral<T>::value, int>::type = 0>
	String toString(T value, int base = 10, int width = 1)
	{
		Expects(base == 10 || base == 16 || base == 8);
		std::stringstream ss;
		if (base == 16) {
			ss.setf(std::ios::hex, std::ios::basefield);
		} else if (base == 8) {
			ss.setf(std::ios::oct, std::ios::basefield);
		}
		if (width > 1) {
			ss << std::setfill('0') << std::setw(width);
		}
		ss << value;
		return ss.str();
	}

	template <typename T, typename std::enable_if<!std::is_integral<T>::value && !std::is_floating_point<T>::value, int>::type = 0>
	String toString(const T& value)
	{
		return ToStringConverter<typename std::remove_cv<T>::type>()(value);
	}



	template <typename T>
	T fromString(const String& value)
	{
		return FromStringConverter<T>()(value);
	}



	template <typename T, typename std::enable_if<!std::is_same<T, String>::value, int>::type = 0>
	String operator+ (const String& lhp, const T& rhp)
	{
		return lhp + toString<typename std::remove_cv<T>::type>(rhp);
	}

	template <typename T, typename std::enable_if<!std::is_same<T, String>::value, int>::type = 0>
	String operator+ (const T& lhp, const String& rhp)
	{
		return toString<typename std::remove_cv<T>::type>(lhp) + rhp;
	}

}
