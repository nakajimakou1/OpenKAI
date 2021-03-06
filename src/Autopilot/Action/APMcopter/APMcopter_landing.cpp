#include "APMcopter_landing.h"

namespace kai
{

APMcopter_landing::APMcopter_landing()
{
	m_pAPM = NULL;
	m_pAI = NULL;
	m_target.init();
}

APMcopter_landing::~APMcopter_landing()
{
}

bool APMcopter_landing::init(void* pKiss)
{
	CHECK_F(this->ActionBase::init(pKiss) == false);
	Kiss* pK = (Kiss*) pKiss;
	pK->m_pInst = this;

	F_INFO(pK->v("orientationX", &m_target.m_orientX));
	F_INFO(pK->v("orientationY", &m_target.m_orientY));
	F_INFO(pK->v("timeout", &m_target.m_timeOut));

	return true;
}

bool APMcopter_landing::link(void)
{
	CHECK_F(!this->ActionBase::link());
	Kiss* pK = (Kiss*) m_pKiss;

	string iName = "";
	F_INFO(pK->v("APMcopter_base", &iName));
	m_pAPM = (APMcopter_base*) (pK->parent()->getChildInstByName(&iName));

	F_ERROR_F(pK->v("_AIbase", &iName));
	m_pAI = (_AIbase*) (pK->root()->getChildInstByName(&iName));
	return true;
}

void APMcopter_landing::update(void)
{
	this->ActionBase::update();

	NULL_(m_pAPM);

	landing();
}

void APMcopter_landing::landing(void)
{
	NULL_(m_pAPM);
	NULL_(m_pAI);
	_StreamBase* pStream = m_pAI->m_pStream;
	NULL_(pStream);

	vInt2 cSize;
	vInt2 cCenter;
	vInt2 cAngle;
	pStream->info(&cSize, &cCenter, &cAngle);

	vInt4 targetBB;
	targetBB.init();
	uint64_t frameID = get_time_usec() - m_target.m_timeOut;

	int k = 0;
	for (int i = 0; i < m_pAI->size(); i++)
	{
		OBJECT* pObj = m_pAI->get(i, frameID);
		if (!pObj)
			continue;

		targetBB = pObj->m_bbox;
		m_target.m_targetPos.m_x = pObj->m_bbox.midX();
		m_target.m_targetPos.m_y = pObj->m_bbox.midY();
		k++;
		break;
	}

	if (k <= 0)
	{
		m_target.m_targetPos.init();
		m_target.m_bLocked = false;
		return;
	}

	m_target.m_bLocked = true;

	//Change position to angles
	m_target.m_angleX = ((double)(m_target.m_targetPos.m_x - cCenter.m_x) / (double)cSize.m_x)
			* cAngle.m_x * DEG_RADIAN * m_target.m_orientX;
	m_target.m_angleY = ((double)(m_target.m_targetPos.m_y - cCenter.m_y) / (double)cSize.m_y)
			* cAngle.m_y * DEG_RADIAN * m_target.m_orientY;

	//Send Mavlink command
	m_pAPM->m_pMavlink->landing_target(MAV_DATA_STREAM_ALL, MAV_FRAME_BODY_NED,
			m_target.m_angleX, m_target.m_angleY, 0, 0, 0);

}

bool APMcopter_landing::draw(void)
{
	CHECK_F(!this->ActionBase::draw());
	Window* pWin = (Window*) this->m_pWindow;
	Mat* pMat = pWin->getFrame()->getCMat();
	string msg;

	if (m_target.m_bLocked)
	{
		circle(*pMat, Point(m_target.m_targetPos.m_x, m_target.m_targetPos.m_y),
				pMat->cols * pMat->rows * 0.0001, Scalar(0, 255, 0), 2);

		msg = "Landing_Target: (" + f2str(m_target.m_angleX) + " , "
				+ f2str(m_target.m_angleY) + ")";
	}
	else
	{
		msg = "Landing_Target: Target not found";
	}

	pWin->addMsg(&msg);

	return true;
}

}

