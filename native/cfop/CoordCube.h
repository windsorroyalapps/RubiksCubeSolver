#pragma once

#include "Cube.h"
#include <cstdint>

/**
 * Coordinate representation used by Kociemba two-phase.
 *
 * Phase 1 coordinates:
 *   twist   - corner orientation (0..2186)
 *   flip    - edge orientation   (0..2047)
 *   slice   - UD-slice edge positions (0..494)
 *
 * Phase 2 coordinates (after G1):
 *   corner permutation, edge permutation (equatorial + UD), etc.
 *
 * Full move tables are generated offline; here we compute coordinates
 * directly from the facelet cube so IDA* can be wired next.
 */
struct CoordCube {
    int16_t twist = 0;   // 0..2186
    int16_t flip  = 0;   // 0..2047
    int16_t slice = 0;   // 0..494  (combination of 4 middle-slice edges)

    // Phase-2 (populated once phase-1 done)
    int16_t cornerPerm = 0;
    int16_t edgePerm   = 0;

    static CoordCube fromCube(const Cube& c);

    bool isG1() const { return twist == 0 && flip == 0 && slice == 0; }
};
