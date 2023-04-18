// \brief Declaration of the class StateWalker

#pragma once
#include "State.h"

namespace cho::osbase::statemachine {
    /**
     * \addtogroup PACKAGE_OSSTATE
     * \{
     */

    /**
     * \brief type of walk
     */
    enum class WalkType {
        direct,   //!< direct walk (only direct children are walked)
        recursive //!< the hierarchy is walked
    };

    template <WalkType TWalkType>
    class StateWalker;

    /**
     * \brief specialization for direct
     */
    template <>
    class StateWalker<WalkType::direct> {
    public:
        explicit StateWalker(StatePtr pState = nullptr); //!< ctor

        State::iterator begin() const; //!< start of the walk
        State::iterator end() const;   //!< end of the walk

    private:
        StatePtr m_pState;
    };

    /**
     * \brief specialization for resursive
     */
    template <>
    class StateWalker<WalkType::recursive> {
    public:
        explicit StateWalker(StatePtr pState = nullptr); //!< ctor

        State::prefix_iterator begin() const; //!< start of the walk
        State::prefix_iterator end() const;   //!< end of the walk

    private:
        StatePtr m_pState;
    };
    /** \} */
}; // namespace cho::osbase::statemachine
