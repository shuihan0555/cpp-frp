#ifndef _FRP_UTIL_STORAGE_H_
#define _FRP_UTIL_STORAGE_H_

#include <array>
#include <cstdint>
#include <memory>

namespace frp {
namespace util {

typedef uint64_t revision_type;

constexpr revision_type default_revision = 0;

template<typename T>
struct storage_type {
	const T value;
	revision_type revision;

	storage_type(T &&value, revision_type revision = default_revision)
		: value(std::forward<T>(value)), revision(revision) {}

	auto compare_value(storage_type &storage) const {
		return value == storage.value;
	}
};

template<>
struct storage_type<void> {
	revision_type revision;

	storage_type(revision_type revision) : revision(revision) {}

	auto compare_value(storage_type &storage) const {
		return false;
	}
};

template<typename T, std::size_t DependenciesN>
struct commit_storage_type : storage_type<T> {
	constexpr static std::size_t dependents_size = DependenciesN;
	typedef std::array<revision_type, dependents_size> revisions_type;
	const revisions_type revisions;

	template<typename F>
	static auto make(F &&function, const revisions_type &revisions) {
		return std::make_shared<commit_storage_type<T, DependenciesN>>(function(),
			default_revision, revisions);
	}

	commit_storage_type(T &&value, revision_type revision, const revisions_type &revisions)
		: storage_type<T>(std::forward<T>(value), revision), revisions(revisions) {}

	bool is_newer(const revisions_type &revisions) const {
		return !(revisions < this->revisions);
	}
};

template<std::size_t DependenciesN>
struct commit_storage_type<void, DependenciesN> : storage_type<void> {
	constexpr static std::size_t dependents_size = DependenciesN;
	typedef std::array<revision_type, dependents_size> revisions_type;
	const revisions_type revisions;

	template<typename F>
	static auto make(F &&function, const revisions_type &revisions) {
		function();
		return std::make_shared<commit_storage_type<void, DependenciesN>>(default_revision,
			revisions);
	}

	commit_storage_type(revision_type revision,
		const std::array<revision_type, dependents_size> &revisions)
		: storage_type<void>(revision), revisions(revisions) {}

	bool is_newer(const revisions_type &revisions) const {
		return !(revisions < this->revisions);
	}
};

} // namespace util
} // namespace frp

#endif // _FRP_UTIL_STORAGE_H_