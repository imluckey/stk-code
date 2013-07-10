//  SuperTuxKart - a fun racing game with go-kart
//  Copyright (C) 2006 SuperTuxKart-Team
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 3
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

#include "modes/soccer_world.hpp"

#include <string>
#include <IMeshSceneNode.h>

#include "audio/music_manager.hpp"
#include "io/file_manager.hpp"
#include "karts/abstract_kart.hpp"
#include "karts/kart_model.hpp"
#include "karts/kart_properties.hpp"
#include "karts/rescue_animation.hpp"
#include "physics/physics.hpp"
#include "states_screens/race_gui_base.hpp"
#include "tracks/track.hpp"
#include "tracks/track_object_manager.hpp"
#include "utils/constants.hpp"

//-----------------------------------------------------------------------------
/** Constructor. Sets up the clock mode etc.
 */
SoccerWorld::SoccerWorld() : WorldWithRank()
{
    WorldStatus::setClockMode(CLOCK_CHRONO);
    m_use_highscores = false;
}   // SoccerWorld

//-----------------------------------------------------------------------------
/** Initializes the soccer world. It sets up the data structure
 *  to keep track of points etc. for each kart.
 */
void SoccerWorld::init()
{
    WorldWithRank::init();
    m_display_rank = false;

    // check for possible problems if AI karts were incorrectly added
    if(getNumKarts() > race_manager->getNumPlayers())
    {
        fprintf(stderr, "No AI exists for this game mode\n");
        exit(1);
    }
}   // init

//-----------------------------------------------------------------------------
/** Called then a battle is restarted.
 */
void SoccerWorld::reset()
{
    WorldWithRank::reset();

    m_can_score_points = true;
    memset(m_team_goals, 0, sizeof(m_team_goals));

    initKartList();
}   // reset

//-----------------------------------------------------------------------------
/** Returns the internal identifier for this race.
 */
const std::string& SoccerWorld::getIdent() const
{
    return IDENT_SOCCER;
}   // getIdent

//-----------------------------------------------------------------------------
/** Update the world and the track.
 *  \param dt Time step size.
 */
void SoccerWorld::update(float dt)
{
    WorldWithRank::update(dt);
    WorldWithRank::updateTrack(dt);

    // TODO
}   // update

void SoccerWorld::onCheckGoalTriggered(bool first_goal)
{
    // TODO
    if(m_can_score_points)
        printf("*** GOOOOOOOOOAAAAAAALLLLLL!!!! (team: %d) ***\n", first_goal ? 0 : 1);

    //m_check_goals_enabled = false;    // TODO: remove?

    // Reset original positions for the soccer balls
    TrackObjectManager* tom = getTrack()->getTrackObjectManager();
    assert(tom);

    PtrVector<TrackObject>&   objects = tom->getObjects();
    for(int i=0; i<objects.size(); i++)
    {
        TrackObject* obj = objects.get(i);
        if(!obj->isSoccerBall())
            continue;

        obj->reset();
    }

    //for(int i=0 ; i < getNumKarts() ; i++

    /*if(World::getWorld()->getTrack()->isAutoRescueEnabled() &&
        !getKartAnimation() && fabs(getRoll())>60*DEGREE_TO_RAD &&
                              fabs(getSpeed())<3.0f                )
    {
        new RescueAnimation(this, true);
    }*/

    // TODO: rescue the karts
    // TODO: score a point
}   // onCheckGoalTriggered

//-----------------------------------------------------------------------------
/** The battle is over if only one kart is left, or no player kart.
 */
bool SoccerWorld::isRaceOver()
{
    // for tests : never over when we have a single player there :)
    if (race_manager->getNumPlayers() < 2)
    {
        return false;
    }

    // TODO
    return getCurrentNumKarts()==1 || getCurrentNumPlayers()==0;
}   // isRaceOver

//-----------------------------------------------------------------------------
/** Called when the race finishes, i.e. after playing (if necessary) an
 *  end of race animation. It updates the time for all karts still racing,
 *  and then updates the ranks.
 */
void SoccerWorld::terminateRace()
{
    m_can_score_points = false;
    WorldWithRank::terminateRace();
}   // terminateRace

//-----------------------------------------------------------------------------
/** Returns the data to display in the race gui.
 */
void SoccerWorld::getKartsDisplayInfo(
                           std::vector<RaceGUIBase::KartIconDisplayInfo> *info)
{
    // TODO!!
    /*
    const unsigned int kart_amount = getNumKarts();
    for(unsigned int i = 0; i < kart_amount ; i++)
    {
        RaceGUIBase::KartIconDisplayInfo& rank_info = (*info)[i];

        // reset color
        rank_info.lap = -1;

        AbstractKart* kart = getKart(i);
        switch(kart->getSoccerTeam())
        {
        case SOCCER_TEAM_BLUE:
            rank_info.r = 0.0f;
            rank_info.g = 0.0f;
            rank_info.b = 0.7f;
            break;
        case SOCCER_TEAM_RED:
            rank_info.r = 0.9f;
            rank_info.g = 0.0f;
            rank_info.b = 0.0f;
            break;
        default:
            assert(false && "Soccer team not set to blue or red");
            rank_info.r = 0.0f;
            rank_info.g = 0.0f;
            rank_info.b = 0.0f;
        }
    }
    */
}   // getKartsDisplayInfo

// ----------------------------------------------------------------------------
/** Set position and team for the karts */
void SoccerWorld::initKartList()
{
    const unsigned int kart_amount = m_karts.size();

    // Set kart positions, ordering them by team
    for(unsigned int n=0; n<kart_amount; n++)
    {
        m_karts[n]->setPosition(-1);
    }
    // TODO: remove
/*
    const unsigned int kart_amount = m_karts.size();

    int team_karts_amount[NB_SOCCER_TEAMS];
    memset(team_karts_amount, 0, sizeof(team_karts_amount));

    {
        // Set the kart teams if they haven't been already set by the setup screen
        // (happens when the setup screen is skipped, with 1 player)
        SoccerTeam    round_robin_team = SOCCER_TEAM_RED;
        for(unsigned int n=0; n<kart_amount; n++)
        {
            if(m_karts[n]->getSoccerTeam() == SOCCER_TEAM_NONE)
                m_karts[n]->setSoccerTeam(round_robin_team);

            team_karts_amount[m_karts[n]->getSoccerTeam()]++;

            round_robin_team = (round_robin_team==SOCCER_TEAM_RED ?
                                SOCCER_TEAM_BLUE : SOCCER_TEAM_RED);
        }// next kart
    }

    // Compute start positions for each team
    int team_cur_position[NB_SOCCER_TEAMS];
    team_cur_position[0] = 1;
    for(int i=1 ; i < (int)NB_SOCCER_TEAMS ; i++)
        team_cur_position[i] = team_karts_amount[i-1] + team_cur_position[i-1];

    // Set kart positions, ordering them by team
    for(unsigned int n=0; n<kart_amount; n++)
    {
        SoccerTeam  team = m_karts[n]->getSoccerTeam();
        m_karts[n]->setPosition(team_cur_position[team]);
        team_cur_position[team]++;
    }// next kart
*/
}
