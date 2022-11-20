#include <thread>
#include <cstring>
#include <whisper.h>

bool working = false;
std::thread *s = nullptr;

void _whisper_full(
            struct whisper_context * ctx,
            struct whisper_full_params *params,
            float * samples,
            int n_samples) {
    whisper_full(ctx, *params, samples, n_samples);
    working = false;
    free(params);
    free(samples);
}

extern "C" {
    WHISPER_API bool whisper_full_thread_is_complete() {
        return !working;
    }

    WHISPER_API void whisper_full_thread(
                struct whisper_context * ctx,
                struct whisper_full_params params,
                const float * samples,
                int n_samples) {
        working = true;

        struct whisper_full_params *owned_params = (struct whisper_full_params *) calloc(1, sizeof(struct whisper_full_params));
        memcpy(owned_params, &params, sizeof(struct whisper_full_params));

        float *owned_samples = (float *) calloc(n_samples, sizeof(float));
        memcpy(owned_samples, samples, n_samples * sizeof(float));

        // FIXME leaking memory here but thread needs to survive this method call
        s = new std::thread(_whisper_full, ctx, owned_params, owned_samples, n_samples);
    }
}
