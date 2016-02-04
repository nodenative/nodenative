#ifndef __NATIVE_HELPER_TEMPLATRESEQIND_H__
#define __NATIVE_HELPER_TEMPLATRESEQIND_H__

namespace native {
namespace helper {

template <std::size_t... Ts>
struct TemplateSeqInd {};

template <std::size_t N, std::size_t... Ts>
struct TemplateSeqIndGen : TemplateSeqIndGen<N-1, N-1, Ts...> {};

template<std::size_t... Ts>
struct TemplateSeqIndGen<0, Ts...> : TemplateSeqInd<Ts...> {};

} /* namespace helper */
} /* namespace native */

#endif /* __NATIVE_HELPER_TEMPLATRESEQIND_H__ */
