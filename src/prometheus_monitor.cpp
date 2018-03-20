
#include "prometheus_monitor.h"
#include <prometheus/histogram_builder.h>

using namespace std;


class PrometheusClient
{
public:
	PrometheusClient();
	Registry& GetRegistry() { return *m_Registry; }
private:
	Exposer m_Exposer;
	std::shared_ptr<Registry> m_Registry;
};

PrometheusClient* g_PrometheusClient = nullptr;
PrometheusBenchmarkMonitor* g_PrometheusBenchmarkMonitor = nullptr;

PrometheusClient::PrometheusClient()
	: m_Exposer("127.0.0.1:8011")
{
	m_Registry = make_shared<Registry>();
	m_Exposer.RegisterCollectable(m_Registry);
}


PrometheusBenchmarkMonitor::PrometheusBenchmarkMonitor()
	: m_CreateNewBlockHistogramFamily(nullptr)
{
	//	Set histogram pointers to null
	memset(m_Histograms, 0, HT_SIZE * sizeof(Histogram*));
}

void PrometheusBenchmarkMonitor::Init(prometheus::Registry& registry)
{
	//	 Don't destroy initialized pointers, because everything is owned by registry. Let registry destroy it

	InitCreateNewBlockMetrics(registry);
	InitConnectBlockMetrics(registry);
	InitDisconnectTipMetrics(registry);
	InitConnectTipMetrics(registry);

}

void PrometheusBenchmarkMonitor::InitCreateNewBlockMetrics(prometheus::Registry& registry)
{
	m_CreateNewBlockHistogramFamily = &BuildHistogram()
		.Name("bitcoind_create_new_block_duration")
		.Help("Tracking time duration spent by CreateNewBlock function")
		.Register(registry);
	m_Histograms[HT_CreateNewBlockTotal] = &m_CreateNewBlockHistogramFamily->Add(
		{ { "Name", "Total" }, { "Unit", "time in ms" } }, Histogram::BucketBoundaries());
	m_Histograms[HT_CreateNewBlockPackages] = &m_CreateNewBlockHistogramFamily->Add(
		{ { "Name", "Packages" },{ "Unit", "time in ms" } }, Histogram::BucketBoundaries());
	m_Histograms[HT_CreateNewBlockValidity] = &m_CreateNewBlockHistogramFamily->Add(
		{ { "Name", "Validity" },{ "Unit", "time in ms" } }, Histogram::BucketBoundaries());
}

void PrometheusBenchmarkMonitor::InitConnectBlockMetrics(prometheus::Registry& registry)
{
	m_ConnectBlockHistogramFamily = &BuildHistogram()
		.Name("bitcoind_connect_block_duration")
		.Help("Tracking time duration spent by ConnectBlock function")
		.Register(registry);
	m_Histograms[HT_ConnectBlockSanityChecks] = &m_ConnectBlockHistogramFamily->Add(
		{ { "Name", "SanityChecks" },{ "Unit", "time in ms" } }, Histogram::BucketBoundaries());
	m_Histograms[HT_ConnectBlockForkChecks] = &m_ConnectBlockHistogramFamily->Add(
		{ { "Name", "ForkChecks" },{ "Unit", "time in ms" } }, Histogram::BucketBoundaries());
	m_Histograms[HT_ConnectBlockConnect] = &m_ConnectBlockHistogramFamily->Add(
		{ { "Name", "Connect" },{ "Unit", "time in ms" } }, Histogram::BucketBoundaries());
	m_Histograms[HT_ConnectBlockVerify] = &m_ConnectBlockHistogramFamily->Add(
		{ { "Name", "Verify" },{ "Unit", "time in ms" } }, Histogram::BucketBoundaries());
	m_Histograms[HT_ConnectBlockIndexWriting] = &m_ConnectBlockHistogramFamily->Add(
		{ { "Name", "IndexWriting" },{ "Unit", "time in ms" } }, Histogram::BucketBoundaries());
	m_Histograms[HT_ConnectBlockCallbacks] = &m_ConnectBlockHistogramFamily->Add(
		{ { "Name", "Callbacks" },{ "Unit", "time in ms" } }, Histogram::BucketBoundaries());
}

void PrometheusBenchmarkMonitor::InitDisconnectTipMetrics(prometheus::Registry& registry)
{
	m_DisconnectTipHistogramFamily = &BuildHistogram()
		.Name("bitcoind_disconnect_tip_duration")
		.Help("Tracking time duration spent by DisconnectTip function")
		.Register(registry);
	m_Histograms[HT_DisconnectTip] = &m_DisconnectTipHistogramFamily->Add(
		{ { "Name", "DisconnectBlock" },{ "Unit", "time in ms" } }, Histogram::BucketBoundaries());
}

void PrometheusBenchmarkMonitor::InitConnectTipMetrics(prometheus::Registry& registry)
{
	m_ConnectTipHistogramFamily = &BuildHistogram()
		.Name("bitcoind_connect_tip_duration")
		.Help("Tracking time duration spent by ConnectTip function")
		.Register(registry);
	m_Histograms[HT_ConnectTipLoadBlock] = &m_ConnectTipHistogramFamily->Add(
		{ { "Name", "LoadBlock" },{ "Unit", "time in ms" } }, Histogram::BucketBoundaries());
	m_Histograms[HT_ConnectTipConnectFailed] = &m_ConnectTipHistogramFamily->Add(
		{ { "Name", "ConnectFailed" },{ "Unit", "time in ms" } }, Histogram::BucketBoundaries());
	m_Histograms[HT_ConnectTipConnectFailedTotal] = &m_ConnectTipHistogramFamily->Add(
		{ { "Name", "ConnectFailedTotal" },{ "Unit", "time in ms" } }, Histogram::BucketBoundaries());
	m_Histograms[HT_ConnectTipFlush] = &m_ConnectTipHistogramFamily->Add(
		{ { "Name", "Flush" },{ "Unit", "time in ms" } }, Histogram::BucketBoundaries());
	m_Histograms[HT_ConnectTipWriteChainstate] = &m_ConnectTipHistogramFamily->Add(
		{ { "Name", "WriteChainstate" },{ "Unit", "time in ms" } }, Histogram::BucketBoundaries());
	m_Histograms[HT_ConnectTipPostProcess] = &m_ConnectTipHistogramFamily->Add(
		{ { "Name", "PostProcess" },{ "Unit", "time in ms" } }, Histogram::BucketBoundaries());
	m_Histograms[HT_ConnectTipConnectSuccess] = &m_ConnectTipHistogramFamily->Add(
		{ { "Name", "ConnectSuccess" },{ "Unit", "time in ms" } }, Histogram::BucketBoundaries());
	m_Histograms[HT_ConnectTipConnectSuccessTotal] = &m_ConnectTipHistogramFamily->Add(
		{ { "Name", "ConnectSuccessTotal" },{ "Unit", "time in ms" } }, Histogram::BucketBoundaries());
}

void PrometheusBenchmarkMonitor::ObserveHistogram(Histogram_Type ht, double value, double multiplier)
{
	m_Histograms[ht]->Observe(value * multiplier);
}


void PrometheusBenchmarkMonitor::ObserveCreateNewBlock(double packages, double validity, double millisecondMultiplier)
{
	assert(m_CreateNewBlockHistogramFamily && "ObserveCreateNewBlock Fatal error! BenchmarkMonitor is not initialized. Call Init before calling this function");
	ObserveHistogram(HT_CreateNewBlockTotal, packages + validity, millisecondMultiplier);
	ObserveHistogram(HT_CreateNewBlockPackages, packages, millisecondMultiplier);
	ObserveHistogram(HT_CreateNewBlockValidity, validity, millisecondMultiplier);
}

void PrometheusBenchmarkMonitor::ObserveConnectBlock(double sanity, double fork, double connect, double verify, double indexWriting, double callbacks, double millisecondMultiplier)
{
	assert(m_ConnectBlockHistogramFamily && "ObserveConnectBlock Fatal error! BenchmarkMonitor is not initialized. Call Init before calling this function");
	ObserveHistogram(HT_ConnectBlockSanityChecks, sanity, millisecondMultiplier);
	ObserveHistogram(HT_ConnectBlockForkChecks, fork, millisecondMultiplier);
	ObserveHistogram(HT_ConnectBlockConnect, connect, millisecondMultiplier);
	ObserveHistogram(HT_ConnectBlockVerify, verify, millisecondMultiplier);
	ObserveHistogram(HT_ConnectBlockIndexWriting, indexWriting, millisecondMultiplier);
	ObserveHistogram(HT_ConnectBlockCallbacks, callbacks, millisecondMultiplier);
}

void PrometheusBenchmarkMonitor::ObserveDisconnectTip(double duration, double millisecondMultiplier)
{
	assert(m_DisconnectTipHistogramFamily && "ObserveDisconnectTip Fatal error! BenchmarkMonitor is not initialized. Call Init before calling this function");
	ObserveHistogram(HT_DisconnectTip, duration, millisecondMultiplier);
}

void PrometheusBenchmarkMonitor::ObserveConnectTipFailed(double loadBlock, double connect, double millisecondMultiplier)
{
	assert(m_DisconnectTipHistogramFamily && "ObserveConnectTip Fatal error! BenchmarkMonitor is not initialized. Call Init before calling this function");
	ObserveHistogram(HT_ConnectTipLoadBlock, loadBlock, millisecondMultiplier);
	ObserveHistogram(HT_ConnectTipConnectFailed, connect, millisecondMultiplier);
}

void PrometheusBenchmarkMonitor::ObserveConnectTipSuccess(double loadBlock, double connect, double flush, double writeChainState, double writePostProcess, double millisecondMultiplier)
{
	assert(m_DisconnectTipHistogramFamily && "ObserveConnectTip Fatal error! BenchmarkMonitor is not initialized. Call Init before calling this function");
	ObserveHistogram(HT_ConnectTipLoadBlock, loadBlock, millisecondMultiplier);
	ObserveHistogram(HT_ConnectTipConnectSuccess, connect, millisecondMultiplier);
	ObserveHistogram(HT_ConnectTipFlush, flush, millisecondMultiplier);
	ObserveHistogram(HT_ConnectTipWriteChainstate, writeChainState, millisecondMultiplier);
	ObserveHistogram(HT_ConnectTipPostProcess, writePostProcess, millisecondMultiplier);
	ObserveHistogram(HT_ConnectTipConnectSuccessTotal, loadBlock + connect + flush + writeChainState + writePostProcess, millisecondMultiplier);
}

void StartPrometheus()
{
	assert(!g_PrometheusClient && "StartPrometheus Fatal error! Multiple StartPrometheus call is detected.");
	g_PrometheusClient = new PrometheusClient();
	auto& registry = g_PrometheusClient->GetRegistry();
	g_PrometheusBenchmarkMonitor = new PrometheusBenchmarkMonitor();
	g_PrometheusBenchmarkMonitor->Init(registry);
}

void StopPrometheus()
{
	assert(g_PrometheusClient && "Stop Fatal error! Multiple StartPrometheus call is detected.");
	delete g_PrometheusBenchmarkMonitor;
	g_PrometheusBenchmarkMonitor = nullptr;
	delete g_PrometheusClient;
	g_PrometheusClient = nullptr;
}
