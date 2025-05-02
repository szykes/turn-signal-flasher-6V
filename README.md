# avr8-project-template

This is a template project for old AT90*, ATmega*, and ATtiny* projects. It uses a simple testing and mocking framework and relies on own `avr-libc` version.

## Building firmware
```
cmake -S . -B cmake
```

```
cmake --build cmake --target app
```

## Building and running test
```
cmake -DTEST=True -S . -B cmake-test
```

```
cmake --build cmake-test --target logic-test
```
