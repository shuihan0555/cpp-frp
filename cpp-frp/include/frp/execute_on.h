#ifndef _FRP_EXECUTE_ON_H_
#define _FRP_EXECUTE_ON_H_

namespace frp {
namespace internal {

template<typename F, typename E>
struct execute_on_type {
	typedef E executor_type;
	typedef F function_type;

	E executor;
	F function;
};

struct execute_immediate_type {
	template<typename F>
	void operator()(F f) const {
		f();
	}
};

template<typename F>
struct from_function_type {
	typedef execute_immediate_type executor_type;
	typedef F function_type;

	static auto executor(F &f) {
		return execute_immediate_type();
	}

	static auto function(F &&f) {
		return std::forward<F>(f);
	}
};

template<typename F, typename E>
struct from_function_type<execute_on_type<F, E>> {
	typedef E executor_type;
	typedef F function_type;

	static auto executor(execute_on_type<F, E> &&f) {
		return std::move(f.executor);
	}

	static auto function(execute_on_type<F, E> &&f) {
		return std::move(f.function);
	}
};

template<typename F>
using get_executor_t = typename from_function_type<F>::executor_type;

template<typename F>
using get_function_t = typename from_function_type<F>::function_type;

template<typename F>
auto get_executor(F &&f) {
	return from_function_type<F>::executor(std::forward<F>(f));
}

template<typename F>
auto get_function(F &&f) {
	return from_function_type<F>::function(std::forward<F>(f));
}

} // namespace internal

template<typename E, typename F>
internal::execute_on_type<F, E> execute_on(E executor, F function) {
	return { std::forward<E>(executor), std::forward<F>(function) };
}

} // namespace frp

#endif // _FRP_EXECUTE_ON_H_