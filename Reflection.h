#pragma once
#include <functional>
#include <iostream>
#include <string>
#include <regex>

#ifdef R_EnableJson
#include "json.hpp"
using namespace nlohmann;
#endif

struct ReflectionMember
{
	std::string name;
	std::string type_name;
	void* value;
	std::size_t offset;

	template<typename T>
	bool is()
	{
		return (typeid(T).raw_name() == type_name);
	}
	template<typename T>
	T get()
	{
		return *reinterpret_cast<T*>(value);
	}
	template<typename T>
	void set(T val)
	{
		*reinterpret_cast<T*>(value) = val;
	}
	void update(void* val)
	{
		this->value = val;
	}

	ReflectionMember(std::string name, void* value, int offset, const char* type_name)
	{
		this->name = name;
		this->value = value;
		this->offset = offset;
		this->type_name = type_name;
	}
#ifdef R_EnableJson
	ReflectionMember(std::string name, void* value, int offset, const char* type_name, std::function<void(ReflectionMember*, json&)> fuckingFuckery, std::function<void(ReflectionMember*, json&)> fuckingFuckFuckery)
	{
		this->name = name;
		this->value = value;
		this->offset = offset;
		this->type_name = type_name;
		this->theFuckening = fuckingFuckery;
		this->theFuckeningPart2 = fuckingFuckFuckery;
	}
#endif

#ifdef R_EnableJson
	std::function<void(ReflectionMember*, json&)> theFuckening = [](ReflectionMember*, json&) {};
	std::function<void(ReflectionMember*, json&)> theFuckeningPart2 = [](ReflectionMember*, json&) {};
	template<typename T>
	bool is(json j)
	{
		try
		{
			T obj = j.get<T>();
			return true;
		}
		catch (...)
		{
			return false;
		}
	}
	json toJson()
	{
		json j;
		theFuckening(this, j);
		return j;
	}
	void fromJson(json j)
	{
		theFuckeningPart2(this, j);
	}
#endif
};
std::regex _thisToClassName(R"(.*\ (\w+)\ .*)");

struct className
{
	std::string name;

	operator std::string()
	{
		return name;
	}

	friend std::ostream& operator<<(std::ostream& os, className& cl)
	{
		os << cl.name;
		return os;
	}

	className(const char* c_name)
	{
		std::string name = c_name;

		std::smatch m;
		if (std::regex_search(name, m, _thisToClassName))
		{
			name = m[1];
		}
		this->name = name;
	}
	className(string name)
	{
		std::smatch m;
		if (std::regex_search(name, m, _thisToClassName))
		{
			name = m[1];
		}
		this->name = name;
	}
	className() {};
};

struct ReflectionClass
{
	className class_name;
	std::vector<ReflectionMember*> members;

	template<typename ... A>
	void manual_fix(A... arg)
	{
		std::vector<void*> args{ { arg... } };
		if (args.size() != members.size())
			throw std::exception("Argument count doesn't match");
		for (int i = 0; members.size() > i; i++)
		{
			members[i]->update(args[i]);
		}
	}

	template<class T>
	void fix(T* clazz)
	{
		for (auto member : members)
		{
			void* fixed_value = (void*)((char*)clazz + member->offset);
			member->update(fixed_value);
		}
	}

	ReflectionMember* get(std::string name)
	{
		for (auto member : members)
		{
			if (member->name == name)
				return member;
		}
		throw exception("Reflection Member not found");
	}

#ifdef R_EnableJson
	json toJson()
	{
		json j;
		j["className"] = this->class_name.name;
		for (auto member : members)
		{
			j[member->name] = member->toJson();
		}
		return j;
	}
	void fromJson(json j)
	{
		if (j["className"].is_string())
		{
			this->class_name = j["className"].get<std::string>();
		}
		for (auto member : members)
		{
			member->fromJson(j[member->name]);
		}
	}
#endif
};

#define REFCLASS ReflectionClass _reflectClass{typeid(this).name(),
#ifdef R_EnableJson
#define REFMEMBERJ(clazz, type, member) new ReflectionMember(#member, &member, offsetof(clazz, clazz::member),typeid(member).raw_name(), [](ReflectionMember* o, json& what) { what = o->get<type>(); }, [](ReflectionMember* o, json& what) { if (o->is<type>(what)) { o->set(what.get<type>()); }} ),
#else
#define REFMEMBER(clazz, member) new ReflectionMember(#member, &member, offsetof(clazz, clazz::member),typeid(member).raw_name()),
#define REFMEMBERJ(clazz, ignore, member) new ReflectionMember(#member, &member, offsetof(clazz, clazz::member),typeid(member).raw_name()),
#endif