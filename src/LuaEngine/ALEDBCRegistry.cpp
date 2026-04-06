#include "ALEDBCRegistry.h"

std::vector<DBCDefinition> dbcRegistry = {
    REGISTER_DBC(GemProperties,    GemPropertiesEntry,    sGemPropertiesStore),
    REGISTER_DBC(Spell,            SpellEntry,            sSpellStore),
    REGISTER_DBC(SkillLine,        SkillLineEntry,        sSkillLineStore),
    REGISTER_DBC(SkillLineAbility, SkillLineAbilityEntry, sSkillLineAbilityStore),
    REGISTER_DBC(SkillTiers,       SkillTiersEntry,       sSkillTiersStore),
};

