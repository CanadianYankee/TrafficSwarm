struct Data {
	uint iSource;
};

RWStructuredBuffer<Data> buff;

[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	uint index = buff.IncrementCounter();
	buff[0].iSource = index;
	index = buff.IncrementCounter();
	buff[1].iSource = index;
}