#pragma once
#include <resources.h>
#include <genesis.h>

//	EActionEffect_NONE, 		//none()							-> does nothing
void actionEffect_none() {
    // do nothing
}
//	EActionEffect_PHYSDMG,		//physDmg(base,mult) 				-> damage = (user.atk * 2 * mult + base) - target.def
void actionEffect_physDmg(RPG_StateBattler* user, RPG_StateBattler* target, s16 base, s16 mult) {
    s16 dmgBase = (user->atk * 2 * mult) / 10;
    s16 damage = 0;
    if (base < 0) {
        damage = (dmgBase - base) - target->def;
        battler_healHp(target, damage, true);
    } else {
        damage = (dmgBase + base) - target->def;
        battler_dmgHp(target, damage, true);
    }
}
//	EActionEffect_PHYSDMGSTAT, 	//physDmgStat(base,mult,stat,val) 	-> same formula as ^. Set stat to val
void actionEffect_physDmgStat(RPG_StateBattler* user, RPG_StateBattler* target, s16 base, s16 mult, s16 stat, s16 val) {
    actionEffect_physDmg(user, target, base, mult);
    if (stat >= 0) {
        battler_applyStat(target, (u8)stat, (char)val);
    }
}
//	EActionEffect_SETSTAT, 		//setStat(s1,v1,s2,v2) 				-> set stat if id !=0
void actionEffect_setStat(RPG_StateBattler* user, RPG_StateBattler* target, s16 stat1, s16 val1, s16 stat2, s16 val2) {
    if (stat1 >= 0) {
        battler_applyStat(target, (u8)stat1, (char)val1);
    }
    if (stat2 >= 0) {
        battler_applyStat(target, (u8)stat2, (char)val2);
    }
}
//	EActionEffect_MAGICDMG,		//magicDmg(base,mult) 				-> damage = (user.mag * 2 * mult + base) - target.mag
void actionEffect_magicDmg(RPG_StateBattler* user, RPG_StateBattler* target, s16 base, s16 mult) {
    s16 dmgBase = (user->mag * 2 * mult) / 10;
    s16 damage = 0;
    if (base < 0) {
        damage = (dmgBase - base) - target->mag;
        battler_healHp(target, damage, true);
    } else {
        damage = (dmgBase + base) - target->mag;
        battler_dmgHp(target, damage, true);
    }
}
//	EActionEffect_MAGICDMGBUFF, //magicDmgBuff(base,mult,buff,v) 	-> same formula as ^. Add v to buff
void actionEffect_magicDmgBuff(RPG_StateBattler* user, RPG_StateBattler* target, s16 base, s16 mult, s16 buff, s16 v) {
    actionEffect_magicDmg(user,target,base,mult);
    battler_applyBuff(target, buff, v);
}
//	EActionEffect_MAGICDMGPIERCE,//magicDmgPierce(base,mult) 		-> damage = (user.mag * 2 * mult + base)
void actionEffect_magicDmgPierce(RPG_StateBattler* user, RPG_StateBattler* target, s16 base, s16 mult) {
    s16 dmgBase = (user->mag * 2 * mult) / 10;
    s16 damage = 0;
    if (base < 0) {
        damage = (dmgBase - base);
        battler_healHp(target, damage, true);
    } else {
        damage = (dmgBase + base);
        battler_dmgHp(target, damage, true);
    }
}
//	EActionEffect_PERCDMG,		//percDmg(perc)						-> damage = target.hp * perc / 100
void actionEffect_percDmg(RPG_StateBattler* user, RPG_StateBattler* target, s16 perc) {
    s16 damage = (target->hp * perc) / 100;
    if (damage < 0) {
        battler_healHp(target, -damage, true);
    } else {
        battler_dmgHp(target, damage, true);
    }
}
//	EActionEffect_BUFF, 		//buff(buff,v) 						-> Add v to buff
void actionEffect_buff(RPG_StateBattler* user, RPG_StateBattler* target, s16 buff, s16 v) {
    battler_applyBuff(target, buff, v);
}
//	EActionEffect_REVIVE, 		//revive(perc)						-> sets HP to perc% to battler with 0HP
void actionEffect_revive(RPG_StateBattler* user, RPG_StateBattler* target, s16 perc) {
    // only affect if character is dead
    if (target->currHp <= 0) {
        s16 damage = (target->hp * perc) / 100;
        if (damage < 0) {
            battler_healHp(target, 1, true);
        } else {
            battler_dmgHp(target, damage, true);
        }
    }
}
