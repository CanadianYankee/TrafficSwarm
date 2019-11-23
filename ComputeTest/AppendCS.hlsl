
struct Data {
	uint source;
};

AppendStructuredBuffer<Data> buff;

[numthreads(32, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	Data data;
	data.source = DTid.x;

	buff.Append(data);
}