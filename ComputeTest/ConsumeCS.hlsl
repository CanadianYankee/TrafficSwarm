struct Data {
	uint iSource;
};

RWStructuredBuffer<Data> buff;

[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	uint index = buff.DecrementCounter();
	buff[2].iSource = index;
	index = buff.DecrementCounter();
	buff[3].iSource = index;
	index = buff.DecrementCounter();
	if (index == 0xffffffff) index = 87;
	buff[4].iSource = index;
}
