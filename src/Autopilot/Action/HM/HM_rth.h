#ifndef OPENKAI_SRC_AUTOPILOT_ACTION_HMRTH_H_
#define OPENKAI_SRC_AUTOPILOT_ACTION_HMRTH_H_

#include "../../../Base/common.h"
#include "../../../Automaton/_Automaton.h"
#include "../../../Algorithm/Filter.h"
#include "../../../Navigation/_Obstacle.h"
#include "../ActionBase.h"
#include "HM_base.h"
#include "../../../AI/_AIbase.h"

namespace kai
{

class HM_rth: public ActionBase
{
public:
	HM_rth();
	~HM_rth();

	bool init(void* pKiss);
	bool link(void);
	void update(void);
	bool draw(void);

private:
	_Obstacle*	m_pUniv;
	HM_base* m_pHM;

	//where target should be on cam coordinate
	double	m_destX;
	double	m_destY;
	double	m_destArea;

	double	m_speedP;
	double	m_steerP;

	OBJECT* m_pTarget;
	kai::Filter* m_pTargetX;
	kai::Filter* m_pTargetY;
	kai::Filter* m_pTargetArea;
	int		m_filterWindow;
	int		m_targetClass;

};

}

#endif

