#pragma once

using namespace DirectX;

class CCourse
{
public:
	typedef std::vector<XMFLOAT2> XMPOLYLINE;
	struct AGENT_SOURCE_SINK {
		AGENT_SOURCE_SINK() : vColor(0.0f, 0.0f, 0.0f) {}
		XMFLOAT3 vColor;
		XMPOLYLINE lineSource;
		XMPOLYLINE lineSink;
		XMFLOAT2 velDir;
		float randLimit;
		float lenSource;
	};

	BOOL LoadFromFile(const CString& strFile);
	BOOL LoadHourglass();
	float GetTotalSourceLength();

	CString m_strName;
	float m_fCourseLength;
	std::vector<XMPOLYLINE> m_vecWalls;
	std::vector<AGENT_SOURCE_SINK> m_vecAgentSS;
};

