
  

<div align="center">
  <p>
    <h1>
      <br />
      reflectpp
    </h1>
    <h4>A single header reflection library</h4>
  </p>
  <p>
    <a href="https://github.com/Git-Curve/CPPReflection/blob/master/LICENSE">
      <img src="https://img.shields.io/github/license/Git-Curve/CPPReflection.svg" alt="License" />
    </a>
  </p>
</div>

## Features
- Reflection on class/struct members
- Serialization with [Json](https://github.com/nlohmann/json)

## Examples
#### Making a class / struct reflectable
```cpp
struct Example
{
	int exampleInt;
	int imNotReflectable;
	
	REFLECTABLE
		REFMEMBER(Example, exampleInt)
	REFLECTABLEEND
};
```
#### Member Access
```cpp
Example example;
example.exampleInt = 100;

std::string className = example.reflectpp.className;
std::string memberName = example.reflectpp.getMemberByName("exampleInt").name;
bool containsExampleInt = example.reflectpp.containsMember("exampleInt");
bool isInt = example.reflectpp.getMemberByName("exampleInt").is<int>();
int value = example.reflectpp["exampleInt"].get<int>();
/*
Results in:
className = "Example"
containsExampleInt = true
isInt = true
value = 100

Members can be accessed through the "getMemberByName" function or by indexing.
However when indexing non existant members a exception is thrown.
*/
```

#### Copies
```cpp
Example original;
original.exampleInt = 100;

Example copy;
memcpy(&copy, &orginal, sizeof(original));

/*
The values are no longer valid due to copying.
To fix this simply call the fix function.
*/
copy.reflectpp.fix(copy);
/*
Now you can continue to work with the copy.
Calling fix is IMPORTANT!
*/
```
Calling the fix function after copying is essential, when not doing so you will encounter problems.
A simple example for this:
```cpp
Example original;
original.exampleInt = 100;

Example copy;
memcpy(&copy, &orginal, sizeof(original));
copy.exampleInt = 0;

int value = copy.reflectpp["exampleInt"].get<int>(); // This will return 100 which is false.

copy.reflectpp.fix(copy); // Quick fix
value = copy.reflectpp["exampleInt"].get<int>(); // This will return 0 which is true.
```

## Json
#### Enable Json Support
```cpp
#define REFLECTPP_JSON
```
#### Reflection with Json Support
```cpp
struct Example
{
	int exampleInt;
	int imNotReflectable;
	
	REFLECTABLE
		REFMEMBERJ(Example, int, exampleInt)
	REFLECTABLEEND
};
```
#### Serialization
```cpp
Example example{200};
json serialized = example.reflectpp.toJson();

Example deserialized;
bool serializationSuccess = deserialized.reflectpp.fromJson(serialized);
/*
serialized.dump() => {"className":"Example","exampleInt":200}
deserialized => Example { exampleInt: 200 }
serializationSuccess => true
*/
```

## Problems and Workarounds
Due to the nature of c++ macros its not possible or atleast I dont know a way to pass datatypes that contain commas as a macro parameter, a basic example of this:

```cpp
struct Example
{
	std::map<int, int> aMap;
	
	REFLECTABLE
		REFMEMBERJ(Example, std::map<int,int>, exampleInt) //This will fail.
	REFLECTABLEEND
};
```

A simple fix to this is:
```cpp
typedef std::map<int, int> intMap;
struct Example
{
	intMap aMap;
	
	REFLECTABLE
		REFMEMBERJ(Example, intMap, exampleInt) //This will work!
	REFLECTABLEEND
};
```