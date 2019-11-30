#include "pch.h"

#include "TrialRun.h"
#include "AgentCourse.h"
#include "RunStatistics.h"
#include "DXUtils.h"	
#include "DrawTimer.h"
#include "AgentGenome.h"

CTrialRun::CTrialRun()
{
}

HRESULT CTrialRun::Intialize(UINT nAgents, const CCourse& cCourse, const CAgentGenome& cGenome)
{
	HRESULT hr = S_OK;
	BOOL bSuccess = TRUE;

	hr = InitDirect3D();
	bSuccess = SUCCEEDED(hr);
	if (!bSuccess) return E_FAIL;

	m_pRunStats = std::make_shared<CRunStatistics>(nAgents);
	m_cCourse = cCourse;
	m_cGenome = cGenome;
	m_pAgentCourse = std::make_shared<CAgentCourse>(false, m_pRunStats);
	hr = m_pAgentCourse->Initialize(m_pD3DDevice, m_pD3DContext, m_cCourse, cGenome);
	if (FAILED(hr))	return hr;

	hr = PrepareShaderConstants();
	if (FAILED(hr)) return hr;

	return S_OK;
}

HRESULT CTrialRun::InitDirect3D()
{
	HRESULT hr = S_OK;

	UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)  
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevel;
	hr = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags,
		NULL, 0, D3D11_SDK_VERSION, &m_pD3DDevice, &featureLevel, &m_pD3DContext);

	if (SUCCEEDED(hr))
	{
		if (featureLevel != D3D_FEATURE_LEVEL_11_0)
			hr = E_FAIL;
	}
	assert(SUCCEEDED(hr));

	return hr;
}

HRESULT CTrialRun::PrepareShaderConstants()
{
	HRESULT hr = S_OK;

	// Frame variables will be set for each render frame
	D3D11_BUFFER_DESC Desc;
	Desc.Usage = D3D11_USAGE_DYNAMIC;
	Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	Desc.MiscFlags = 0;
	Desc.ByteWidth = sizeof(FRAME_VARIABLES);
	hr = m_pD3DDevice->CreateBuffer(&Desc, NULL, &m_pCBFrameVariables);
	if (FAILED(hr)) return hr;
	D3DDEBUGNAME(m_pCBFrameVariables, "FrameVariables CB");

	return hr;
}

BOOL CTrialRun::Run(RUN_RESULTS &results)
{
	// Run the simulation as fast as possible, using artificial time increments
	CDrawTimer cTimer;
	cTimer.Tick();
	float dt = 1.0f / 60.f;
	float T = 0.0f;
	float Tstop = 0.0f;
	BOOL bSuccess = TRUE;
	UINT nAgents = m_pRunStats->GetTotalRunSize();
	float maxRunTime = (float)m_pRunStats->GetTotalRunSize() * m_cCourse.m_fCourseLength / 500.f;

	for(T = dt; bSuccess && T < maxRunTime; T+=dt)
	{
		m_sFrameVariables.g_fElapsedTime = dt;
		m_sFrameVariables.g_fGlobalTime = T;
		m_sFrameVariables.g_iMaxAliveAgents = m_pAgentCourse->GetMaxAlive();

		HRESULT hr = CDXUtils::MapDataIntoBuffer(m_pD3DContext, &m_sFrameVariables, sizeof(m_sFrameVariables), m_pCBFrameVariables);
		if (FAILED(hr))
		{
			assert(false);
			return FALSE;
		}

		bSuccess = m_pAgentCourse->UpdateAgents(m_pCBFrameVariables, dt, T);

		if (m_pAgentCourse->GetNumSpawned() >= nAgents)
		{
			if (Tstop == 0.0f)
			{
				m_pAgentCourse->SetSpawnActive(false);
				Tstop = T;
			}
			if (m_pAgentCourse->GetMaxAlive() == 0)
			{
				break;
			}
		}
	}

	if (bSuccess)
	{
		cTimer.Tick();
		float realTime = cTimer.DeltaTime();
		m_pRunStats->RecordRunResults(results);
		results.strCourseName = m_cCourse.m_strName;
		results.genome = m_cGenome;
		results.fRealTime = realTime;
		results.fSimulatedTime = T;
	}

	return bSuccess;
}

CString CTrialRun::RUN_RESULTS::ToListString(const CString &strSeparator) const
{
	CString strOut, str;
	str.Format(_T("Score = %.1f "), Score()); strOut += str + strSeparator;
	str.Format(_T("%d / %d complete "), nComplete, nAgents); strOut += str + strSeparator;
	str.Format(_T("Avg Life = %.1f "), fAvgLifetime); strOut += str + strSeparator;
	str.Format(_T("Avg agent-agent = %.2f "), fAvgAACollisions); strOut += str + strSeparator;
	str.Format(_T("Avg agent-wall = %.2f "), fAvgAWCollisions); strOut += str + strSeparator;
	str.Format(_T("Failed spawns = %d "), nSpawnFails); strOut += str + strSeparator;
	str.Format(_T("Left escapes = %d "), nLeftEscapes); strOut += str + strSeparator;
	str.Format(_T("Right escapes = %d "), nRightEscapes); strOut += str + strSeparator;
	str.Format(_T("Simulated %.0f sec in %.1f real sec"), fSimulatedTime, fRealTime); strOut += str;

	return strOut;
}

CString CTrialRun::RUN_RESULTS::ToParagraphString(const CString& strSeparator) const
{
	CString strOut, str;
	str.Format(_T("Score = %.1f ; %d / %d complete "), Score(),nComplete, nAgents); strOut += str + strSeparator;
	str.Format(_T("Avg Life = %.1f ; Avg agent-agent = %.2f ; Avg agent-wall = %.2f "), 
		fAvgLifetime, fAvgAACollisions, fAvgAWCollisions); strOut += str + strSeparator;
	str.Format(_T("Failed spawns = %d ; Left escapes = %d ; Right escapes = %d "), 
		nSpawnFails, nLeftEscapes, nRightEscapes); strOut += str + strSeparator;
	str.Format(_T("Simulated %.0f sec in %.1f real sec"), fSimulatedTime, fRealTime); strOut += str;

	return strOut;
}
