
struct Data {
	uint source;
};

RWStructuredBuffer<Data> buff;

[numthreads(32, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	Data data;
	data.source = DTid.x;

	uint index = buff.IncrementCounter();

	buff[index] = data;
}