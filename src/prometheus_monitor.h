#ifndef BITMAIN_PROMETHEUS_MONITOR_H_
#define BITMAIN_PROMETHEUS_MONITOR_H_

#include <cstdint>

#include <prometheus/exposer.h>
#include <prometheus/registry.h>
#include <prometheus/family.h>
#include <prometheus/histogram.h>

using namespace prometheus;

//!  Monitor for -debug=bench
/*!
This class is to handle everything Logged by -debug=bench
*/
class PrometheusBenchmarkMonitor
{
public:

	PrometheusBenchmarkMonitor();
	void Init(prometheus::Registry& registry);

	//!  Monitor for CreateNewBlock duration
	/*!
	//	Observe duration for packages, validity and total
	//	You can pass any time unit, but all value must use the same time unit.
	//	millisecondMultiplier: If the time unit used is not millisecond, then pass the multiplier to convert time unit to millisecond.
	//	examples: 
	//		- if passed value are in seconds, then pass 1000 to millisecondMultiplier
	//		- if passed value are in microseconds, then pass 0.001 to millisecondMultiplier
	*/
	void ObserveCreateNewBlock(double packages, double validity, double millisecondMultiplier = 1.f);

	//!  Monitor for ConnectBlock duration
	/*!
	//	Observe duration for sanity check, fork check, connect, verify, index writing, callbacks.
	//	millisecondMultiplier: Read additional info from ObserveCreateNewBlock
	*/
	void ObserveConnectBlock(double sanity, double fork, double connect, double verify, double indexWriting, double callbacks, double millisecondMultiplier = 1.f);

	//!  Monitor for DisconnectTip duration
	/*!
	//	Observe duration for disconnect tip
	//	millisecondMultiplier: Read additional info from ObserveCreateNewBlock
	*/
	void ObserveDisconnectTip(double duration, double millisecondMultiplier = 1.f);

	//!  Monitor for ConnectTipFailed duration
	/*!
	//	Observe duration for connect tip failed
	//	millisecondMultiplier: Read additional info from ObserveCreateNewBlock
	*/
	void ObserveConnectTipFailed(double loadBlock, double connect, double millisecondMultiplier = 1.f);

	//!  Monitor for ConnectTipSuccess duration
	/*!
	//	Observe duration for connect tip success
	//	millisecondMultiplier: Read additional info from ObserveCreateNewBlock
	*/
	void ObserveConnectTipSuccess(double loadBlock, double connect, double flush, double writeChainState, double writePostProcess, double millisecondMultiplier = 1.f);


private:

	//	use Histogram_Type enum as array index.
	enum Histogram_Type : int32_t
	{
		HT_INVALID = -1,
		//	Note: Add new type below this line

		//	CreateNewBlock section
		HT_CreateNewBlockTotal,
		HT_CreateNewBlockPackages,
		HT_CreateNewBlockValidity,

		//	ConnectBlock section
		HT_ConnectBlockSanityChecks,
		HT_ConnectBlockForkChecks,
		HT_ConnectBlockConnect,
		HT_ConnectBlockVerify,
		HT_ConnectBlockIndexWriting,
		HT_ConnectBlockCallbacks,

		//	DisconnectTip Section
		HT_DisconnectTip,

		//	ConnectTip Section
		HT_ConnectTipLoadBlock,
		HT_ConnectTipConnectFailed,
		HT_ConnectTipConnectFailedTotal,
		HT_ConnectTipFlush,
		HT_ConnectTipWriteChainstate,
		HT_ConnectTipPostProcess,
		HT_ConnectTipConnectSuccess,
		HT_ConnectTipConnectSuccessTotal,


		//	Note: Add new type above this line
		HT_SIZE,
		HT_START = HT_INVALID + 1
	};

	void InitCreateNewBlockMetrics(prometheus::Registry& registry);
	void InitConnectBlockMetrics(prometheus::Registry& registry);
	void InitDisconnectTipMetrics(prometheus::Registry& registry);
	void InitConnectTipMetrics(prometheus::Registry& registry);

	void ObserveHistogram(Histogram_Type ht, double value, double multiplier);

	Family<Histogram>* m_CreateNewBlockHistogramFamily;
	Family<Histogram>* m_ConnectBlockHistogramFamily;
	Family<Histogram>* m_DisconnectTipHistogramFamily;
	Family<Histogram>* m_ConnectTipHistogramFamily;
	Histogram* m_Histograms[HT_SIZE];

};

extern PrometheusBenchmarkMonitor* g_PrometheusBenchmarkMonitor;

void StartPrometheus();
void StopPrometheus();

#endif	// BITMAIN_PROMETHEUS_MONITOR_H_