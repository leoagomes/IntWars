DamageType = {
   Physical = 0,
   True = 1,
   Magical = 2
}

DamageSource = {
   Attack = 0,
   Spell = 1,
   Summoner_Spell = 2,
   Passive = 3
}

AttackType = {
   Radial = 0,
   Melee = 1,
   Target = 2
}

MoveOrder = {
   Move = 0,
   AttackMove = 1
}

FieldMaskOne = {
    FM1_Gold =                   0x00000001,
    FM1_Gold_Total =             0x00000002,
    FM1_Spells_Enabled =         0x00000004, -- Bits: 0-3 -> Q-R, 4-9 -> Items, 10 -> Trinket
    FM1_SummonerSpells_Enabled = 0x00000010 -- Bits: 0 -> D, 1 -> F
}

FieldMaskTwo = {
    FM2_Base_Ad =           0x00000020, -- champ's base ad that increase every level. No item bonus should be added here
    FM2_Base_Ap =           0x00000040,
    FM2_Crit_Chance =       0x00000100, -- 0.5 = 50%
    FM2_Armor =             0x00000200,
    FM2_Magic_Armor =       0x00000400,
    FM2_Hp5 =               0x00000800,
    FM2_Mp5 =               0x00001000,
    FM2_Range =             0x00002000,
    FM2_Bonus_Ad_Flat =     0x00004000, -- AD flat bonuses
    FM2_Bonus_Ad_Pct =      0x00008000, -- AD percentage bonuses. 0.5 = 50%
    FM2_Bonus_Ap_Flat =     0x00010000, -- AP flat bonuses
    FM2_Atks_multiplier =   0x00080000, -- Attack speed multiplier. If set to 2 and champ's base attack speed is 0.600, then his new AtkSpeed becomes 1.200
    FM2_cdr =               0x00400000, -- Cooldown reduction. 0.5 = 50%   
    FM2_Armor_Pen_Flat =    0x01000000,
    FM2_Armor_Pen_Pct =     0x02000000, -- Armor pen. 0.5 = 50%
    FM2_Magic_Pen_Flat =    0x04000000,
    FM2_Magic_Pen_Pct =     0x08000000,
    FM2_LifeSteal =         0x10000000, --Life Steal. 0.5 = 50%
    FM2_SpellVamp =         0x20000000, --Spell Vamp. 0.5 = 50%
    FM2_Tenacity =          0x40000000 --Tenacity. 0.5 = 50%
}

FieldMaskThree = {
    FM3_Armor_Pen_Pct = 0x00000001, --Armor pen. 1.7 = 30% -- These are probably consequence of some bit ops
    FM3_Magic_Pen_Pct = 0x00000002 --Magic pen. 1.7 = 30%
}

FieldMaskFour = {
    FM4_CurrentHp =     0x00000001,
    FM4_CurrentMana =   0x00000002,
    FM4_MaxHp =         0x00000004,
    FM4_MaxMp =         0x00000008,
    FM4_exp =           0x00000010,
    FM4_Vision1 =       0x00000100,
    FM4_Vision2 =       0x00000200,
    FM4_Speed =         0x00000400,
    FM4_ModelSize =     0x00000800,
    FM4_Level =         0x00004000, -- Champion Level
    FM4_Invuln =        0x00010000 -- Unk -> Transparent-ish Life bar when changed:: MAYBE IF UNIT IS TARGETABLE
}

FieldMaskFive = {
}

BuffDuration = {
   Eternal = 0,
   Temporary = 1
}

BuffFlag = {
   Buff = 0,
   Debuff = 1,
   Disable = 2,

   Airborn = 3,
   Blid = 4,
   Disarm = 5,
   Forced = 6,
   Root = 7,
   Silence = 8,
   Slow = 9,
   Stasis = 10,
   Stun = 11,
   Suspension = 12,
   Suppression = 13,

   Transfer = 14
}
