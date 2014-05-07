Building AdvSpec plugin
==============================

Notice
-
AdvSpec is not yet available for Linux & OS X.

Requirements
-
* **[Windows]** Visual Studio or Visual Express C++
* source-sdk-2013 - https://github.com/ValveSoftware/source-sdk-2013/

Building
-  
AdvSpec now uses VPC to generate correct project settings, though it needs some setup.  
  
- Clone/copy the advspec repo in to `source-sdk-2013/mp/src/utils/`  
- Copy the scripts you want from the scripts folder up to mp/src  
- Open `mp/src/vpc_scripts/projects.vgc` and add the following to it:  
```
$Project "advspec
{
    "utils\advspec\advspec.vpc" [$WIN32||$POSIX]
}
```  
Now double-click `createadvspec.bat` and you should see advspec.sln show up.  
By default it creates projects for VS 2012, but you can change this by changing the number in the .bat file.
