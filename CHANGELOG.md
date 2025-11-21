# Changelog

All notable changes to the DA7281 HAL project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.0.0] - 2024-11-21

### Added
- Initial release of DA7281 HAL for Qorvo DWM3001C platform
- Core HAL implementation with full DA7281 register support
- Thread-safe I2C communication layer with FreeRTOS mutex protection
- Multi-device support (up to 4 devices on 2 I2C buses)
- LRA configuration with automatic parameter calculation
- Power control with GPIO enable pin
- Operation mode control (Inactive, DRO, PWM, RTWM, ETWM, Standby)
- Override amplitude control for DRO mode
- Self-test functionality
- Chip ID and revision reading
- Comprehensive error handling with specific error codes
- Parameter validation macros
- Extensive logging support (NRF_LOG integration)
- MISRA-C compliant coding standards
- Complete Doxygen documentation
- Unit test framework with Unity
- Integration test examples
- CI/CD pipeline with GitHub Actions
- CMake build system
- Example application (haptics_demo.c)
- Architecture documentation
- Coding standards guide
- Testing guide

### Features
- **Thread Safety**: FreeRTOS mutex protection for all I2C operations
- **Multi-Device**: Support for up to 4 DA7281 devices
- **Configurable**: Compile-time configuration options
- **Robust**: Comprehensive error handling and parameter validation
- **Well-Documented**: Doxygen comments, architecture docs, coding standards
- **Tested**: Unit tests, integration tests, CI/CD pipeline
- **Professional**: MISRA-C compliance, static analysis, code coverage

### Hardware Support
- Qorvo DWM3001C (nRF52833)
- Dialog/Renesas DA7281 haptic driver
- LRA actuators (tested with 170Hz, 6.75Ω)

### Dependencies
- Qorvo DWM3001CDK SDK v1.1.1 or later
- FreeRTOS (included in Qorvo SDK)
- nRF5 SDK TWI driver (included in Qorvo SDK)
- ARM GCC Toolchain 10.3-2021.10 or later

### Documentation
- README.md - Project overview and quick start
- docs/ARCHITECTURE.md - Design and architecture details
- docs/CODING_STANDARDS.md - Coding standards and MISRA-C compliance
- docs/TESTING.md - Testing strategy and procedures
- docs/Doxyfile - Doxygen configuration for API documentation

### Build System
- CMake 3.20+ support
- Ninja build system integration
- Debug and Release configurations
- Optional unit tests (ENABLE_TESTS)
- Optional code coverage (ENABLE_COVERAGE)
- Optional examples (ENABLE_EXAMPLES)

### CI/CD
- GitHub Actions workflow
- Automated build (Debug and Release)
- Unit test execution
- Static analysis (cppcheck)
- MISRA-C compliance checking
- Code coverage reporting
- Documentation generation
- Artifact publishing

### Known Limitations
- Only DRO (Direct Register Override) mode fully implemented
- PWM, RTWM, ETWM modes require additional implementation
- SNP waveform memory programming not yet implemented
- Interrupt support not yet implemented
- Auto-resonance tracking not yet implemented

### Future Enhancements
- Waveform memory support (SNP programming)
- Embedded waveform mode (ETWM) implementation
- Real-time waveform mode (RTWM) implementation
- PWM mode implementation
- Interrupt support (IRQ pin)
- Auto-resonance tracking
- Power management optimization
- Additional LRA profiles

## [Unreleased]

### Planned for v1.1.0
- [ ] Waveform memory programming
- [ ] ETWM mode implementation
- [ ] Interrupt support
- [ ] Additional example applications
- [ ] Performance benchmarks
- [ ] Power consumption measurements

---

## Version History

- **v1.0.0** (2024-11-21) - Initial release

---

## License

MIT License

---

## Notes

This changelog follows the [Keep a Changelog](https://keepachangelog.com/) format.

### Categories
- **Added**: New features
- **Changed**: Changes in existing functionality
- **Deprecated**: Soon-to-be removed features
- **Removed**: Removed features
- **Fixed**: Bug fixes
- **Security**: Security fixes

