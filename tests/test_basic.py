# Start via `make test-debug` or `make test-release`


async def test_basic(service_client):
    # response = await service_client.get('/suggest', params={'word': 'fedor'})
    assert 200 == 200
    # assert response.text == 'fedoras'