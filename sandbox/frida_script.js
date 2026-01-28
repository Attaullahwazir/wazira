// Log calls to eval and document.write

// Hook eval
var orig_eval = eval;
eval = function() {
    send("eval called with: " + arguments[0]);
    return orig_eval.apply(this, arguments);
};

// Hook document.write if document exists
if (typeof document !== 'undefined' && document.write) {
    var orig_write = document.write;
    document.write = function() {
        send("document.write called with: " + arguments[0]);
        return orig_write.apply(this, arguments);
    };
} 