// Copyright 2016 Yahoo Inc. Licensed under the terms of the Apache 2.0 license. See LICENSE in the project root.
package com.yahoo.vespa.config.proxy;

import org.junit.Before;
import org.junit.Rule;
import org.junit.Test;
import org.junit.rules.TemporaryFolder;

import static org.hamcrest.CoreMatchers.is;
import static org.junit.Assert.assertThat;

/**
 * @author hmusum
 */
public class DelayedResponseHandlerTest {

    private final MapBackedConfigSource source = new MapBackedConfigSource(UpstreamConfigSubscriberTest.MockClientUpdater.create());

    @Rule
    public TemporaryFolder temporaryFolder = new TemporaryFolder();

    @Before
    public void setup() {
        source.clear();
        source.put(ProxyServerTest.fooConfig.getKey(), ProxyServerTest.createConfigWithNextConfigGeneration(ProxyServerTest.fooConfig, 0));
    }

    @Test
    public void basic() {
        ConfigTester tester = new ConfigTester();
        ConfigProxyStatistics statistics = new ConfigProxyStatistics();
        DelayedResponses delayedResponses = new DelayedResponses(statistics);
        final MockRpcServer mockRpcServer = new MockRpcServer();
        final MemoryCache memoryCache = new MemoryCache();
        memoryCache.put(ProxyServerTest.fooConfig);
        final DelayedResponseHandler delayedResponseHandler = new DelayedResponseHandler(delayedResponses, memoryCache, mockRpcServer);
        delayedResponses.add(new DelayedResponse(tester.createRequest(ProxyServerTest.fooConfig, 0)));
        delayedResponses.add(new DelayedResponse(tester.createRequest(ProxyServerTest.fooConfig, 1200000))); // should not be returned yet
        delayedResponses.add(new DelayedResponse(tester.createRequest(ProxyServerTest.errorConfig, 0)));  // will not give a config when resolving
        delayedResponseHandler.checkDelayedResponses();

        assertThat(mockRpcServer.responses, is(1L));
    }

}