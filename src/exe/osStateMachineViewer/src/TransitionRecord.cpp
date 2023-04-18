// \brief Declaration of the struct TransitionRecord

#include "TransitionRecord.h"

namespace nssm = cho::osbase::statemachine;

namespace cho::osbase::statemachineviewer {

    bool KeyTransitionRecord::operator<(const KeyTransitionRecord &other) const {
        if (processName != other.processName) {
            return processName < other.processName;
        }

        if (smName != other.smName) {
            return smName < other.smName;
        }

        return smId < other.smId;
    }
} // namespace cho::osbase::statemachineviewer
