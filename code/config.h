#ifndef CONFIG_H
#define CONFIG_H

// test mode
#define TEST false
#define SPAWN_ENEMY true

#include <iostream>

// config files for constants

// Object types
enum class ObjectType {
    Player,
    Turret,
    Enemy,
    Bullet,
    Explosion,
    UI
};

// game state
#define IN_GAME 10
#define END_SCREEN_LOSE 11
#define END_SCREEN_WIN 12
#define START_SCREEN 13

// game properties
#define STARTING_ENEMY_COUNT 10

#define NON_SPAWNABLE_DISTANCE_FROM_PLAYER  2.0f
#define MIN_SPAWNABLE_DISTANCE_FROM_PLAYER 15.0f
#define MAX_SPAWNABLE_DISTANCE_FROM_PLAYER 30.0f
#define MIN_SPAWNABLE_DISTANCE_FROM_COLLECTIBLE 1.0f
#define MAX_SPAWNABLE_DISTANCE_FROM_COLLECTIBLE 15.0f

#define COLLECIBLE_ENEMY_SPAWN_RANGE 30.0f      // max distance to player for winning collectible to start spawning enemies

#define MIN_ENEMY_SPAWN_TIME 0.2f
#define MAX_ENEMY_SPAWN_TIME 5.0f

// enemy

#define MAX_ENEMY_COUNT 500
#define SPAWN_ATTEMPT 3     // number of attempts per enemy spawn when enemy spawned within other game objects
#define ENEMY_DESPAWN_RANGE 100.0f

// enemy types
enum class EnemyType {
    MELEE_ENEMY,
    RANGED_ENEMY,
    CHARGE_ENEMY
};

// enemy state
enum class EnemyState {
    IDLE,
    WANDER,
    ALERT,
    ENGAGE,
    CHARGE
};

// enemy properties

// spawn weight
#define MELEE_ENEMY_SPAWN_WEIGHT 6
#define RANGED_ENEMY_SPAWN_WEIGHT 3
#define CHARGE_ENEMY_SPAWN_WEIGHT 1.5f

// melee
#define MELEE_ENEMY_SIZE 1.5f
#define MELEE_ENEMY_HEALTH 100.0f

#define MELEE_ENEMY_MELEE_DAMAGE 10.0f
#define SAW_DAMAGE 15.0f

#define MELEE_ENEMY_WEIGHT 10.0f
#define SAW_WEIGHT 10.0f

#define MELEE_ENEMY_IDLE_DETECTION_RADIUS 10.0f
#define MELEE_ENEMY_ENGAGE_DETECTION_RADIUS 15.0f

#define MELEE_ENEMY_SPEED 9.5f
#define MELEE_ENEMY_TURN_SPEED 5.0f     // in radians
#define ARM_TURN_SPEED 1.0f

#define MELEE_ENEMY_WANDER_CHANCE 0.5f
#define MELEE_ENEMY_WANDER_RANGE 5.0f

// ranged
#define RANGED_ENEMY_SIZE 1.0f
#define RANGED_ENEMY_HEALTH 30.0f

#define RANGED_ENEMY_MELEE_DAMAGE 2.0f

#define RANGED_ENEMY_WEIGHT 5.0f

#define RANGED_ENEMY_IDLE_DETECTION_RADIUS 15.0f
#define RANGED_ENEMY_ENGAGE_DETECTION_RADIUS 25.0f

#define RANGED_ENEMY_SPEED 7.0f
#define RANGED_ENEMY_TURN_SPEED 8.0f     // in radians

#define RANGED_ENEMY_WANDER_CHANCE 0.5f
#define RANGED_ENEMY_WANDER_RANGE 5.0f

#define RANGED_ENEMY_MIN_ENGAGE_RANGE 6.0f
#define RANGED_ENEMY_MAX_ENGAGE_RANGE 10.0f

#define RANGED_ENEMY_RATE_OF_FIRE 2.0f

// charge
#define CHARGE_ENEMY_SIZE 3.0f
#define CHARGE_ENEMY_HEALTH 150.0f

#define CHARGE_ENEMY_MELEE_DAMAGE 5.0f

#define CHARGE_ENEMY_WEIGHT 15.0f

#define CHARGE_ENEMY_IDLE_DETECTION_RADIUS 10.0f
#define CHARGE_ENEMY_ENGAGE_DETECTION_RADIUS 25.0f

#define CHARGE_ENEMY_SPEED 10.0f
#define CHARGE_ENEMY_TURN_SPEED 5.0f     // in radians

#define CHARGE_ENEMY_WANDER_CHANCE 0.5f
#define CHARGE_ENEMY_WANDER_RANGE 5.0f

#define CHARGE_ENEMY_CHARGE_RANGE 10.0f
#define CHARGE_ENEMY_CHARGE_PREPARE_TIME 0.5f   // seconds
#define CHARGE_ENEMY_CHARGE_TIME 0.5f   // seconds
#define CHARGE_ENEMY_CHARGE_COOLDOWN_TIME 1.0f  // seconds
#define CHARGE_ENEMY_CHARGE_SPEED 50.0f
#define CHARGE_ENEMY_CHARGE_DAMAGE 30.0f

#define CHARGE_ENEMY_MAX_ENGAGE_RANGE 15.0f

#define CHARGE_ENEMY_RATE_OF_FIRE 0.2f;

// player

enum class PlayerControlType
{
    FORWARD,
    BACKWARD,
    STRAFE_LEFT,
    STRAFE_RIGHT,
    TURN_LEFT,
    TURN_RIGHT
};


// bullet

enum class BulletType
{
    NORMAL,
    MACHINE_BULLET,
    CANNONBALL,
    BIGBULLET,
    HOMING
};


// turret

enum class TurretType
{
    NORMAL,
    MACHINE_GUN,
    CANNON,
    HOMING
};


// collectibles

enum class CollectibleType
{
    FOUR_TURRETS,
    MACHINE_GUN,
    SHOTGUN,
    WIN_CONDITION,
    CANNON,
    HOMING
};

#define COLLECTIBLE_SPAWN_TIME 20.0f
#define COLLECTIBLE_SPAWN_RANGE 20.0f   // from player

#define FOUR_TURRETS_SPAWN_WEIGHT 4
#define MACHINE_GUN_SPAWN_WEIGHT 8
#define SHOTGUN_SPAWN_WEIGHT 5
#define HOMING_SPAWN_WEIGHT 7
#define CANNON_SPAWN_WEIGHT 3

#define MAX_WIN_COLLECTIBLE_SPAWN_RANGE 120.0f  // from 0, 0, 0

enum class WeaponShootKey
{
    PRIMARY,
    SECONDARY
};


// particles
#define BULLET_PARTICLE 10
#define EXPLOSION_PARTICLE 11


// UI
#define HEALTH_BAR 1000
#define ARROW 1001

#endif