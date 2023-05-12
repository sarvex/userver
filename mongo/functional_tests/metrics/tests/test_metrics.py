async def test_metrics_smoke(monitor_client, force_metrics_to_appear):
    metrics = await monitor_client.metrics()
    assert len(metrics) > 1


async def test_metrics_portability(service_client, force_metrics_to_appear):
    warnings = await service_client.metrics_portability()
    # TODO use separate paths for total metrics
    warnings.pop('label_name_mismatch', None)
    assert not warnings


def _is_mongo_metric(line: str) -> bool:
    if 'mongo' not in line and 'distlock' not in line:
        return False

    # These errors sometimes appear during service startup,
    # it's tedious to reproduce them for metrics tests.
    return (
        'mongo.pool.conn-init.errors' not in line
        or 'mongo_error=network' not in line
        and 'mongo_error=cluster-unavailable' not in line
    )


def _normalize_metrics(metrics: str) -> str:
    result = [line for line in metrics.splitlines() if _is_mongo_metric(line)]
    result.sort()
    return '\n'.join(result)


def _hide_metrics_values(metrics: str) -> str:
    return '\n'.join(line.rsplit(' ', 2)[0] for line in metrics.splitlines())


async def test_metrics(monitor_client, load, force_metrics_to_appear):
    ground_truth = _normalize_metrics(load('metrics_values.txt'))
    all_metrics = await monitor_client.metrics_raw(output_format='graphite')
    all_metrics = _normalize_metrics(all_metrics)
    all_metrics_paths = _hide_metrics_values(all_metrics)
    ground_truth_paths = _hide_metrics_values(ground_truth)

    assert all_metrics_paths == ground_truth_paths, (
        '\n===== Service metrics start =====\n'
        f'{all_metrics}\n'
        '===== Service metrics end =====\n'
    )
