#ifndef TEST_DATALOADER_HPP
#define TEST_DATALOADER_HPP

namespace my::test::test_dataloader {

void should_construct_and_iterate();
void should_return_correct_batch_size();
void should_reset();
void should_handle_remaining_batch();
void should_shuffle_with_reset();

} // namespace my::test::test_dataloader

#endif // TEST_DATALOADER_HPP
