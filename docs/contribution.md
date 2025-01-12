# Contribution:
Thank you for expressing interest and helping with this project! Here is a guide for contribution.

## Git Commit guidlines:
- feat: The new feature you're adding to a particular application
- fix: A bug fix
- update: Refactoring or updating a specific section of the codebase
- clean: Regular code maintenance.
- test: Everything related to testing
- docs: Everything related to documentation

## Branch names:
Use `camelCasing`. Prefix your branch name with the type of change you are making, followed by the specific change. Seperate the two with a `/` delimeter. \

e.g.
```
fix/loaderMemoryLeak
```

## Semantics:
- Declare variables in `camelCase`.
- Declare classes in `UpperCamelCase`.
- Header files should be suffixed with `.h`, not `.hpp`.
- Do not use `_variable` or `m_variable` - use the `this` keyword.
- Avoid using raw pointers declared with `new`, use smart pointers like `std::unique_ptr<T>()` or `std::shared_ptr<T>()` instead.
- Use snake_case for filenames and prefix new filenames with `lazarus_`

## CI:
There are two continuous-integration workflows upstream which you should be aware of.
1. `create-release.yml`:
  - This workflow creates and publishes a new release.
  - To trigger, create a tag and then push to main with the `SemVer` number prefixed with a `v`, like so:
```
git tag v1.0.0
git push origin v1.0.0 
```

2. `changelog.yml`
  - This workflow creates changelogs based on the commit messages merged upstream since the last release. This workflow triggers on merge to branch `main`.
  - On merge be sure to insert the `SemVer` tag.

## File structure:
The projects dependencies are *not* checked-in locally for reasons due to liscencing and redistribution. They should instead be installed as system libraries on your machine. 
There are downloads available for each of the libraries required to run lazarus in the [Resources](./docs/resources.md) section. Alternatively the system-specific package managers like `vcpkg`, `apt` and `homebrew` should have binaries available for most of them.

**To install a library on a Unix system:** \
Simply move all header (`.h` / `.hpp`) files to 
```
/usr/local/include/
```

Move the corresponding library (`.dylib` / `.so`) files to
```
/usr/local/lib/
```

On a Linux system you will also have to run
```
ldconfig
```

**To install a library on a Windows system:** \
move all dynamically linked libraries (`.dll`) to the `/bin` directory located in the directory of MSVC's compiler. For me this is
```
c:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.42.34433\bin
```

move all of the statically linked libraries (`.lib`) to the `/lib` directory
```
c:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.42.34433\lib
```

move all of the headers (`.h` / `.hpp`) to the `/include` folder
```
c:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.42.34433\include
```