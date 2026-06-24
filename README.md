# Axis ACAP TIP Workshop Exercises

This repository is an exercise version of `axis-acap-tip-workshop`.
It keeps the same workshop folder structure, Dockerfiles, manifests, Makefiles, assets, and helper files, but the primary C file in each buildable example has been reduced to a TODO skeleton.

The corresponding `README.md` in each example contains:

- what file to edit
- the implementation snippet to paste into the C file
- build commands
- a short verification note

The original `axis-acap-tip-workshop` repository remains the complete reference. Try the exercises first, then compare with the original workshop if you get stuck.

## Get The Exercises

Clone this exercise repository before the workshop:

```sh
git clone <exercise-repository-url>
cd axis-acap-tip-workshop-exercises
```

If you also want the complete reference repository locally, clone it next to this repository:

```sh
cd ..
git clone <reference-repository-url> axis-acap-tip-workshop
cd axis-acap-tip-workshop-exercises
```

## Common Build Flow

From any example directory containing a `Dockerfile`:

```sh
docker build --tag <example-name> --build-arg ARCH=aarch64 .
docker cp $(docker create <example-name>):/opt/app ./build
```

The generated `.eap` package will be in `./build`.



## Learning Order

1. `axis-intro/minimal-app`
2. `parameter/`
3. `vapix/`
4. `event/`
5. `overlay/` and `overlay2/`
6. `bbox/`
7. `vdo/`
8. `larod/`
9. `webserver-fastcgi/`
10. `webserver-proxy/`
