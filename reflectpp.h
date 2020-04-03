#ifndef REFLECTPP_H
#define REFLECTPP_H
#if !((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || __cplusplus >= 201703L)
#error C++17 required
#endif
#ifdef REFLECTPP_JSON
#if __has_include(<json.hpp>)
#include <json.hpp>
#endif
#ifndef INCLUDE_NLOHMANN_JSON_HPP_
#error nlohmann json could not be found.
#endif
using namespace nlohmann;
#endif

#include <regex>
#include <string>
#include <functional>

namespace reflectpp
{

	struct ReflectionMember
	{
		std::string name;
		std::string type;
		void* value;
		std::size_t offset;
#ifdef REFLECTPP_JSON
		std::function<void(ReflectionMember&, json&)> _toJson;
		std::function<bool(ReflectionMember&, const json&)> _fromJson;
#endif
		template <typename T>
		T get() noexcept(false)
		{
			return *reinterpret_cast<T*>(value);
		}
		template <typename T>
		bool is() noexcept
		{
			return typeid(T).raw_name() == type;
		}
		template <typename T>
		void set(T val) noexcept(false)
		{
			*reinterpret_cast<T*>(value) = val;
		}
		void update(void* value) noexcept
		{
			this->value = value;
		}

#ifdef REFLECTPP_JSON
		template <typename T>
		bool is(const json& what) noexcept(false)
		{
			try
			{
				T obj = what.get<T>();
				return true;
			}
			catch (...)
			{
				return false;
			}
		}
		json toJson() noexcept
		{
			json res;
			_toJson(*this, res);
			return res;
		}
		bool fromJson(json& from) noexcept
		{
			return _fromJson(*this, from);
		}
#endif
	};

	namespace internal
	{
		inline std::regex _thisToClassName(R"(.*\ (\w+)\ .*)");
		class className
		{
		private:
			std::string name;
		public:
			className(const std::string& name) noexcept
			{
				this->name = name;
				std::smatch match;
				if (std::regex_search(this->name, match, _thisToClassName))
				{
					this->name = match[1];
				}
			}
			className(const char* name) noexcept : className(std::string(name)) {}
			friend std::ostream& operator<<(std::ostream& os, className& cl) noexcept
			{
				os << cl.name;
				return os;
			}
			operator const std::string() noexcept
			{
				return name;
			}
		};
	}

	struct ReflectionClass
	{
		internal::className className;
		std::vector<ReflectionMember> members;

		template<class T>
		void fix(const T& clazz) noexcept
		{
			for (auto& member : members)
			{
				void* fixedValue = (void*)((char*)&clazz + member.offset);
				member.update(fixedValue);
			}
		}
		const bool containsMember(const std::string& name) noexcept
		{
			for (auto& member : members)
			{
				if (member.name == name)
					return true;
			}
			return false;
		}
		ReflectionMember& getMemberByName(const std::string& name) noexcept(false)
		{
			for (auto& member : members)
			{
				if (member.name == name)
					return member;
			}
			throw std::exception("Member not found");
		}
		ReflectionMember& operator[](const std::string& name)
		{
			return getMemberByName(name);
		}
#ifdef REFLECTPP_JSON
		json toJson() noexcept
		{
			json res;
			res["className"] = (std::string)this->className;
			for (auto& member : members)
			{
				auto val = member.toJson();
				res[member.name] = val;
			}
			return res;
		}
		const bool fromJson(const json& from) noexcept
		{
			if (from["className"].is_string())
			{
				this->className = from["className"].get<std::string>();
			}
			else
			{
				return false;
			}
			for (auto& member : members)
			{
				if (from.count(member.name) <= 0) return false;
				auto jsonMember = from[member.name];
				if (jsonMember.is_null()) return false;
				if (!member.fromJson(jsonMember)) return false;
			}
			return true;
		}
#endif
	};
}

#define REFLECTABLE reflectpp::ReflectionClass reflectpp{typeid(this).name(), {
#ifdef REFLECTPP_JSON
#define REFMEMBERJ(clazz, type, member) reflectpp::ReflectionMember{#member, typeid(member).raw_name(), &member, offsetof(clazz, clazz::member), [](reflectpp::ReflectionMember& member, json& result) {result = member.get<type>(); }, [](reflectpp::ReflectionMember& member, const json& from) { if (member.is<type>(from)) { member.set(from.get<type>()); return true; }  return false; } },
#else
#define REFMEMBER(clazz, member) reflectpp::ReflectionMember{#member, typeid(member).raw_name(), &member, offsetof(clazz, clazz::member)},
#endif
#define REFLECTABLEEND }};
#endif