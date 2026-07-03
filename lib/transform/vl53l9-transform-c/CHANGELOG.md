# Changelog

## [0.4.0] - 2025-08-14
- Align with new media-object and vl53l9-utlils interfaces
- Add sharpener algorithm and associated bypass controls
- Enable support for pointcloud output
- Align to reference isp v2.4.7
- Add resolution for i3c output format (no padding)
## [0.3.2] - 2025-04-18
- Update license
- Minor bugfixes
## [0.3.1] - 2025-03-06
- Refactor raw data parsing
- Fix issue related to effective spads
## [0.3.0] - 2025-03-03
- Update reflectance correction factor
- Temporary use dss hardcoded values
- Output signal by default when amplitude is requested
- Fix ambient_attenuation value in confidence computation
- Fix ratenorm configuration parameters
- Fix allocation of reference_coeffs map
## [0.2.0] - 2025-02-18
- Rename ipp-c to transform-c
- Reuse of media-c definitions instead of custom ones
- Add ratenorm and reflectance algorithms
- Force users to configure input stream before output ones
- Add support for dss in extract
## [0.1.0] - 2025-01-28
- Initial release
