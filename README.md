


<div align="center">
  <p>
    <h1>
      <br />
      CPPReflection
    </h1>
    <h4>A simple cpp-reflection library.</h4>
  </p>
  <p>
    <a href="https://github.com/Git-Curve/CPPReflection/blob/master/LICENSE">
      <img src="https://img.shields.io/github/license/Git-Curve/CPPReflection.svg?style=flat-square" alt="License" />
    </a>
  </p>
</div>

## Features
- Simple reflection
- Serialization with [Json](https://github.com/nlohmann/json)

## Examples
```cpp
struct TestStruct
{
	std::vector<int> testVec;
	string testString;
	int testInt;

	REFCLASS
	{
		REFMEMBER(TestStruct, testVec)
		REFMEMBER(TestStruct, testString)
		REFMEMBER(TestStruct, testInt)
	}};
};

TestStruct test;
```

This is an example class, the reflection-data is obtained through the `_reflectClass` variable.

The ClassName of the class can be obtained by doing the following:
```cpp
string className = test._reflectClass.class_name;
```
You can iterate over the ClassMembers by doing this:
```cpp
for (auto member : test._reflectClass.members)
{
	string name = member->name;
	string type_name = member->type_name;
	
	if (member->is<int>())
	{
		int value = member->get<int>();
	}
}
```

Should you copy the class, you're required to fix it, because the Value-Pointers of the ReflectionMembers will point to a wrong location.
To fix the ReflectionMembers use the fix function.
```cpp
test._reflectClass.fix(&test);
```

### Json Support
This library has support for the nlohmann json library.
It allows you to serialize classes.

To enable Json support
```cpp
#define R_EnableJson
```

<b>CAUTION: </b> When enabling the Json Support you have to specify the dataType of the Class Members in the Reflection Macro like this:
```cpp
struct TestStruct
{
	std::vector<int> testVec;
	string testString;
	int testInt;

	REFCLASS
	{
		REFMEMBERJ(TestStruct, std::vector<int>, testVec)
		REFMEMBERJ(TestStruct, string, testString)
		REFMEMBERJ(TestStruct, int, testInt)
	}};
};

```

You can serialize a class by doing the following:
```cpp
json serialized = test._reflectClass.toJson();
```
and deserialize:
```cpp
json j = ...;
TestStruct deserialize;
deserialize._reflectClass.fromJson(j);
```