#ifndef INCLUDE_GUNS
#define INCLUDE_GUNS

typedef enum t_gun_type {
	GUN_DESERTEAGLE,
	GUN_MP5,
	GUN_NOVA,

	GUN_ALL,
} gun_type;

typedef struct t_gun {
	gun_type type;
	char* name;
	int magazine_size;
	int max_ammunition;
	float bullet_spread;
	int bullets_per_shot;
	float shots_per_second;
	int damage;
} gun;

gun guns[GUN_ALL] = {
	{GUN_DESERTEAGLE, "Desert Eagle", 8, 64, 0.0f, 1, 4.0f, 350},
	{GUN_MP5, "MP5", 30, 120, 0.1f, 1, 10.0f, 150},
	{GUN_NOVA, "Nova", 12, 80, 0.2f, 3, 1.2f, 600},
};

image* get_image_of_gun(gun_type type);
gun get_gun_by_type(gun_type type);

#endif