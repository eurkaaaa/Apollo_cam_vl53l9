# Changelog

## [0.8.1] - 2025-09-11
- Update fw patch
## [0.8.0] - 2025-08-05
- Update strategy to retrieve calibration data
## [0.7.1] - 2025-06-02
- Update fw patch
## [0.7.0] - 2025-04-30
- Add vl53l9_set_com_config & vl53l9_get_com_config methods
- fix warning
## [0.6.0] - 2025-04-18
- Add vl53l9_get_status method
- Add vl53l9_get_async methods
- Update license
- Set default signaling mode to interrupt pad
## [0.5.1] - 2025-03-06
- Fix dss buffer sizes
## [0.5.0] - 2025-02-28
- Update fw patch and align settings to LLSDK v2.2.33
- Rework methods for exposure management
- Implement crop to avoid having resolutions with odd lines
- Perform one single read to retrieve depth, amplitude and ambient
## [0.4.0] - 2025-01-28
- Rework get_frame method to dump data as is instead of parsing it
- Rework get_calibration method to dump otp and dss_effective_spad data instead of parsing it
## [0.3.0] - 2025-01-23
- Enable external clock configuration (through platform layer)
- Fix implementation of set_address method
## [0.2.0] - 2024-12-10
- Enable support for CSI
- Rework the way VDDA and VDDIO voltages are configured (through platform layer)
## [0.1.0-beta] - 2024-10-01
- Enable support for CS1 modules (aligned to LLSDK v2.1.6)
- Add methods to configure VDDA and VDDIO voltages
## [0.1.0-alpha] - 2024-07-19
- Initial release enabling support for ES3 modules
