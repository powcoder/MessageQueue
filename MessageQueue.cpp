https://powcoder.com
代写代考加微信 powcoder
Assignment Project Exam Help
Add WeChat powcoder
https://powcoder.com
代写代考加微信 powcoder
Assignment Project Exam Help
Add WeChat powcoder
#include <cassert>
#include "IMessageQueueManager.h"

void RunUnitTests(IMessageQueueManager& in_manager);

int main()
{
	auto manager = CreateMessageQueueManager();

	// Verify message queue manager instance
	RunUnitTests(*manager);

	return 0;
}

void RunUnitTests(IMessageQueueManager& in_manager)
{
	const std::string queueA = "queueA";
	const std::string queueB = "queueB";
	const std::string queueC = "queueC";

	const auto assertSubscriberCount = [&](const std::string& q, int c) {
		int qc;
		assert(in_manager.GetSubscriptionCount(q, qc));
		assert(qc == c);
	};

	const auto assertMessage = [&](SubscriptionHandle sh, const std::string& m) {
		std::string qm;
		assert(in_manager.GetNextMessage(sh, qm));
		assert(qm == m);
	};

	// Verify basic creation functionality
	SubscriptionHandle shA0, shB0;
	assert(in_manager.CreateMessageQueue(queueA));
	assert(in_manager.CreateSubscription(queueA, shA0));
	assert(!in_manager.CreateSubscription(queueB, shB0));
	assert(in_manager.CreateMessageQueue(queueB));
	assert(!in_manager.CreateMessageQueue(queueA));
	assert(in_manager.CreateSubscription(queueB, shB0));
	assertSubscriberCount(queueA, 1);
	assertSubscriberCount(queueB, 1);

	// Verify basic message posting
	assert(in_manager.PostMessage(queueA, "amsg1"));
	assert(in_manager.PostMessage(queueB, "bmsg1"));
	assert(!in_manager.PostMessage(queueC, "cmsg1"));
	assert(in_manager.PostMessage(queueA, "amsg2"));
	assert(in_manager.PostMessage(queueB, "bmsg2"));
	assert(!in_manager.PostMessage(queueA, ""));

	// Verify basic message reading
	assertMessage(shA0, "amsg1");
	assertMessage(shB0, "bmsg1");
	assertMessage(shA0, "amsg2");
	assertMessage(shB0, "bmsg2");
	assertMessage(shA0, "");
	assertMessage(shB0, "");

	// Verify Interleaved Post/Create
	SubscriptionHandle shA1, shB1;
	assert(in_manager.PostMessage(queueA, "amsg3"));
	assert(in_manager.PostMessage(queueB, "bmsg3"));
	assert(in_manager.PostMessage(queueA, "amsg4"));
	assert(in_manager.PostMessage(queueB, "bmsg4"));
	assert(in_manager.CreateSubscription(queueA, shA1));
	assertSubscriberCount(queueA, 2);
	assertSubscriberCount(queueB, 1);
	assert(in_manager.CreateSubscription(queueB, shB1));
	assert(in_manager.PostMessage(queueA, "amsg5"));
	assertSubscriberCount(queueA, 2);
	assertSubscriberCount(queueB, 2);

	// Verify Interleaved Read/Post
	assertMessage(shA1, "amsg5");
	assertMessage(shB1, "");
	assert(in_manager.PostMessage(queueB, "bmsg5"));
	assert(in_manager.PostMessage(queueB, "bmsg6"));
	assertMessage(shA1, "");
	assertMessage(shA0, "amsg3");
	assertMessage(shA0, "amsg4");
	assertMessage(shB0, "bmsg3");
	assert(in_manager.PostMessage(queueA, "amsg6"));
	assertMessage(shA1, "amsg6");
	assertMessage(shB0, "bmsg4");
	assertMessage(shA0, "amsg5");
	assertMessage(shB0, "bmsg5");
	assertMessage(shA0, "amsg6");
	assertMessage(shB0, "bmsg6");
	assertMessage(shA0, "");
	assertMessage(shB0, "");
	assertMessage(shB1, "bmsg5");
	assertMessage(shB1, "bmsg6");
	assertMessage(shB1, "");

	// Verify separate new queue
	SubscriptionHandle shC0;
	assert(!in_manager.PostMessage(queueC, "cmsg1"));
	assert(!in_manager.PostMessage(queueC, "cmsg2"));
	assert(in_manager.CreateMessageQueue(queueC));
	assert(in_manager.PostMessage(queueC, "cmsg1"));
	assert(in_manager.PostMessage(queueC, "cmsg2"));
	assertSubscriberCount(queueC, 0);
	assert(in_manager.CreateSubscription(queueC, shC0));
	assert(in_manager.PostMessage(queueC, "cmsg3"));
	assertMessage(shC0, "cmsg3");
	assert(in_manager.PostMessage(queueC, "cmsg4"));
	assertMessage(shC0, "cmsg4");
	assertMessage(shC0, "");
	assert(in_manager.PostMessage(queueC, "cmsg5"));
	assertMessage(shC0, "cmsg5");
	assertSubscriberCount(queueC, 1);

	// Verify still cleaned up
	assertMessage(shA0, "");
	assertMessage(shA1, "");
	assertMessage(shB0, "");
	assertMessage(shB1, "");
	assertMessage(shC0, "");
	assertSubscriberCount(queueA, 2);
	assertSubscriberCount(queueB, 2);
	assertSubscriberCount(queueC, 1);
}
