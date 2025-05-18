class ZenMapMarkerListEntry
{
	protected string m_Label;     // Display name
	protected vector m_Position;  // Map position
	protected int m_Source;       // 0 = server, 1 = client config, 2 = map item
	protected int m_Index;        // Original index in source array

	void ZenMapMarkerListEntry(string label, vector pos, int source, int index)
	{
		m_Label		= label;
		m_Position	= pos;
		m_Source	= source;
		m_Index		= index;
	}

	string GetLabel()
	{
		return m_Label;
	}

	vector GetPosition()
	{
		return m_Position;
	}

	int GetSourceArray()
	{
		return m_Source;
	}

	int GetArrayIndex()
	{
		return m_Index;
	}
}