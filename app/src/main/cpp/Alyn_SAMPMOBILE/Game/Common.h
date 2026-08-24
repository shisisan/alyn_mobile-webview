#pragma once

enum eBoneTag {
	BONE_UNKNOWN = -1,

	BONE_ROOT = 0, // Normal or Root, both are same
	BONE_PELVIS = 1,
	BONE_SPINE = 2,
	BONE_SPINE1 = 3,
	BONE_NECK = 4,
	BONE_HEAD = 5,
	BONE_L_BROW = 6,
	BONE_R_BROW = 7,
	BONE_JAW = 8,

	BONE_R_CLAVICLE = 21,
	BONE_R_UPPER_ARM = 22,
	BONE_R_FORE_ARM = 23,
	BONE_R_HAND = 24,
	BONE_R_FINGER = 25,
	BONE_R_FINGER_01 = 26,

	BONE_L_CLAVICLE = 31,
	BONE_L_UPPER_ARM = 32,
	BONE_L_FORE_ARM = 33,
	BONE_L_HAND = 34,
	BONE_L_FINGER = 35,
	BONE_L_FINGER_01 = 36,

	BONE_L_THIGH = 41,
	BONE_L_CALF = 42,
	BONE_L_FOOT = 43,
	BONE_L_TOE_0 = 44,

	BONE_R_THIGH = 51,
	BONE_R_CALF = 52,
	BONE_R_FOOT = 53,
	BONE_R_TOE_0 = 54,

	BONE_BELLY = 201,

	BONE_L_BREAST = 302,
	BONE_R_BREAST = 301,

	BONE_MAX_ID = 303,
	MAX_BONE_NUM = 32
};

//-----------------------------------------------------------

#define    VEHICLE_SUBTYPE_CAR           1 // CAutomobile
#define    VEHICLE_SUBTYPE_BIKE          2
#define    VEHICLE_SUBTYPE_HELI          3
#define    VEHICLE_SUBTYPE_BOAT          4
#define    VEHICLE_SUBTYPE_PLANE         5
#define    VEHICLE_SUBTYPE_PUSHBIKE      6 // CBmx
#define    VEHICLE_SUBTYPE_TRAIN         7
#define    VEHICLE_SUBTYPE_MONSTERTRUCK  8
#define    VEHICLE_SUBTYPE_QUADBIKE      9
#define    VEHICLE_SUBTYPE_TRAILER       10

//-----------------------------------------------------------

#define TRAIN_PASSENGER_LOCO             538
#define TRAIN_FREIGHT_LOCO               537
#define TRAIN_PASSENGER                  570
#define TRAIN_FREIGHT                    569
#define TRAIN_TRAM                       449
#define HYDRA                            520

//-----------------------------------------------------------

#define ACTION_WASTED                    55
#define ACTION_DEATH                     54
#define ACTION_INCAR                     50
#define ACTION_NORMAL                    1
#define ACTION_SCOPE                     12
#define ACTION_NONE                      0

//-----------------------------------------------------------

#define WEAPON_FIST                      0
#define WEAPON_BRASSKNUCKLE              1
#define WEAPON_GOLFCLUB                  2
#define WEAPON_NITESTICK                 3
#define WEAPON_KNIFE                     4
#define WEAPON_BAT                       5
#define WEAPON_SHOVEL                    6
#define WEAPON_POOLSTICK                 7
#define WEAPON_KATANA                    8
#define WEAPON_CHAINSAW                  9
#define WEAPON_DILDO                     10
#define WEAPON_DILDO2                    11
#define WEAPON_VIBRATOR                  12
#define WEAPON_VIBRATOR2                 13
#define WEAPON_FLOWER                    14
#define WEAPON_CANE                      15
#define WEAPON_GRENADE                   16
#define WEAPON_TEARGAS                   17
#define WEAPON_MOLOTOV                    18
#define WEAPON_ROCKET                    19
#define WEAPON_ROCKET_HS                 20
#define WEAPON_FREEFALLBOMB              21
#define WEAPON_COLT45                    22
#define WEAPON_SILENCED                  23
#define WEAPON_DEAGLE                    24
#define WEAPON_SHOTGUN                   25
#define WEAPON_SAWEDOFF                  26
#define WEAPON_SHOTGSPA                  27
#define WEAPON_UZI                       28
#define WEAPON_MP5                       29
#define WEAPON_AK47                      30
#define WEAPON_M4                        31
#define WEAPON_TEC9                      32
#define WEAPON_RIFLE                     33
#define WEAPON_SNIPER                    34
#define WEAPON_ROCKETLAUNCHER            35
#define WEAPON_HEATSEEKER                36
#define WEAPON_FLAMETHROWER              37
#define WEAPON_MINIGUN                   38
#define WEAPON_SATCHEL                   39
#define WEAPON_BOMB                      40
#define WEAPON_SPRAYCAN                  41
#define WEAPON_FIREEXTINGUISHER          42
#define WEAPON_CAMERA                    43
#define WEAPON_NIGHTVISION               44
#define WEAPON_INFRARED                  45
#define WEAPON_PARACHUTE                 46
#define WEAPON_ARMOUR                    47
#define WEAPON_VEHICLE                   49
#define WEAPON_HELIBLADES                50
#define WEAPON_EXPLOSION                 51
#define WEAPON_DROWN                     53
#define WEAPON_COLLISION                 54

//-----------------------------------------------------------

#define WEAPON_MODEL_BRASSKNUCKLE       331 // was 332
#define WEAPON_MODEL_GOLFCLUB           333
#define WEAPON_MODEL_NITESTICK          334
#define WEAPON_MODEL_KNIFE              335
#define WEAPON_MODEL_BAT                336
#define WEAPON_MODEL_SHOVEL             337
#define WEAPON_MODEL_POOLSTICK          338
#define WEAPON_MODEL_KATANA             339
#define WEAPON_MODEL_CHAINSAW           341
#define WEAPON_MODEL_DILDO              321
#define WEAPON_MODEL_DILDO2             322
#define WEAPON_MODEL_VIBRATOR           323
#define WEAPON_MODEL_VIBRATOR2          324
#define WEAPON_MODEL_FLOWER             325
#define WEAPON_MODEL_CANE               326
#define WEAPON_MODEL_GRENADE            342 // was 327
#define WEAPON_MODEL_TEARGAS            343 // was 328
#define WEAPON_MODEL_MOLOTOV            344 // was 329
#define WEAPON_MODEL_COLT45             346
#define WEAPON_MODEL_SILENCED           347
#define WEAPON_MODEL_DEAGLE             348
#define WEAPON_MODEL_SHOTGUN            349
#define WEAPON_MODEL_SAWEDOFF           350
#define WEAPON_MODEL_SHOTGSPA           351
#define WEAPON_MODEL_UZI                352
#define WEAPON_MODEL_MP5                353
#define WEAPON_MODEL_AK47               355
#define WEAPON_MODEL_M4                 356
#define WEAPON_MODEL_TEC9               372
#define WEAPON_MODEL_RIFLE              357
#define WEAPON_MODEL_SNIPER             358
#define WEAPON_MODEL_ROCKETLAUNCHER     359
#define WEAPON_MODEL_HEATSEEKER         360
#define WEAPON_MODEL_FLAMETHROWER       361
#define WEAPON_MODEL_MINIGUN            362
#define WEAPON_MODEL_SATCHEL            363
#define WEAPON_MODEL_BOMB               364
#define WEAPON_MODEL_SPRAYCAN           365
#define WEAPON_MODEL_FIREEXTINGUISHER   366
#define WEAPON_MODEL_CAMERA             367
#define WEAPON_MODEL_NIGHTVISION        368    // newly added
#define WEAPON_MODEL_INFRARED           369    // newly added
#define WEAPON_MODEL_JETPACK            370    // newly added
#define WEAPON_MODEL_PARACHUTE          371

#define OBJECT_PARACHUTE                3131
#define OBJECT_CJ_CIGGY                 1485
#define OBJECT_DYN_BEER_1               1486
#define OBJECT_CJ_BEER_B_2              1543
#define OBJECT_CJ_PINT_GLASS            1546
