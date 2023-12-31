#ifndef _EXTRUDE_H_
#define _EXTRUDE_H_

#define EXTRUDER_ID      {"E0", "E1", "E2", "E3", "E4", "E5"}
// Extrude speed mm/min
#define EXTRUDE_SLOW_SPEED      100
#define EXTRUDE_NORMAL_SPEED    200
#define EXTRUDE_FAST_SPEED      300

extern bool pause_extrude_flag;

void menuExtrude(void);
void menuExtrude2();
#endif
