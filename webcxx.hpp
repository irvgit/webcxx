#pragma once

#if !defined(__clang__)
#error unsupported compiler, requires llvm/clang + emscripten
#endif

#if !defined(EMSCRIPTEN)
#error emscripten not found
#endif

#if __cplusplus < 202302L
    #error out of date c++ version, compile with -stdc++=2c
#endif

#define web_nodiscard [[nodiscard]]
#define web_mayunused [[maybe_unused]]
#define web_always_inline [[gnu::always_inline]]
#define web_flatten [[gnu::flatten]]
#define web_packed [[gnu::packed]]
#define web_assume(...) [[assume(__VA_ARGS__)]]
#define web_unreachable std::unreachable();
#define web_final final
#define web_noexcept(...) noexcept(__VA_ARGS__)
#define web_explicit(...) explicit(__VA_ARGS__)
#define web_delete(...) delete(__VA_ARGS__)

#include <algorithm>
#include <array>
#include <charconv>
#include <functional>
#include <iostream>
#include <ranges>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

#include <emscripten/emscripten.h>

namespace web {
    inline namespace cxx {
        using namespace std::literals;
        namespace detail {
            template <typename tp_type_t, typename tp_enforce_as_type_t>
            concept enforce = std::same_as<std::remove_cvref_t<tp_type_t>, tp_enforce_as_type_t>;
            template <typename tp_type_t, typename tp_enforce_as_type_t>
            concept range_of = std::ranges::range<tp_type_t> && std::same_as<std::ranges::range_value_t<tp_type_t>, tp_enforce_as_type_t>;
            template <typename tp_type_t, typename tp_enforce_as_type_t>
            concept range2d_of = std::ranges::range<std::remove_cvref_t<tp_type_t>> && std::same_as<std::ranges::range_value_t<std::ranges::range_value_t<tp_type_t>>, tp_enforce_as_type_t>;
            template <typename tp_type_t, typename tp_to_type_t>
            concept range_of_type_or_convertible_or_toable =
                std::ranges::range<tp_type_t> &&
                (std::same_as<tp_type_t, tp_to_type_t> ||
                std::convertible_to<std::ranges::range_value_t<tp_type_t>, tp_to_type_t> ||
                requires (std::ranges::range_value_t<tp_type_t> p_a) { std::ranges::to<tp_to_type_t>(p_a); });
            template <typename tp_type_t, typename tp_enforce_type_t>
            concept single_or_range_of = std::same_as<tp_type_t, tp_enforce_type_t> || range_of<tp_type_t, tp_enforce_type_t>;
            template <typename tp_type_t, typename tp_construct_type_t>
            concept can_construct = std::constructible_from<tp_construct_type_t, tp_type_t>;
            auto constexpr relativity_type_strings = std::array<const char*, 2>{
                "px", "%"
            };
            auto constexpr flex_vertical_allignment_type_strings = std::array<const char*, 3>{
                "bottom", "middle", "top"
            };
            auto constexpr non_flex_item_allignment_type_strings = std::array<const char*, 5>{
                "stretch", "flex-start", "flex-end", "center", "baseline"
            };
            auto constexpr flex_display_type_strings = std::array<const char*, 2>{
                "flex", "inline-flex"
            };
            auto constexpr non_flex_display_type_strings = std::array<const char*, 5>{
                "inline", "block", "inline-block", "grid", "inline-grid"
            };
            auto constexpr flex_content_justification_type_strings= std::array<const char*, 11>{
                "none",         "flex-start",   "flex-end",     "center", "space-between",
                "space-around", "space-evenly", "start", "end", "left",   "right"
            };
            auto constexpr non_flex_item_justification_type_strings = std::array<const char*, 15>{
                "none",     "normal",         "stretch",       "center",      "start",
                "end",      "self-start",     "self-end",      "left",        "right",
                "baseline", "first-baseline", "last-baseline", "safe-center", "unsafe-center"
            };
            auto constexpr style_key_strings = std::array<const char*, 108>{
                "span",                        "span:hover",                                    "span:active",                                    "span:focus",
                "button",                      "button:hover",                                  "button:active",                                  "button:focus",
                "input[type='text']",          "input[type='text']:hover",                      "input[type='text']:active",                      "input[type='text']:focus",
                "input[type='password']",      "input[type='password']:hover",                  "input[type='password']:active",                  "input[type='password']:focus",
                "input[type='number']",        "input[type='number']:hover",                    "input[type='number']:active",                    "input[type='number']:focus",
                "input[type='range']",         "input[type='range']:hover",                     "input[type='range']:active",                     "input[type='range']:focus",
                "input[type='search']",        "input[type='search']:hover",                    "input[type='search']:active",                    "input[type='search']:focus",
                "input[type='checkbox']",      "input[type='checkbox']:hover",                  "input[type='checkbox']:active",                  "input[type='checkbox']:checked",
                "input[type='radio']",         "input[type='radio']:hover",                     "input[type='radio']:active",                     "input[type='radio']:checked",
                "textarea",                    "textarea:hover",                                "textarea:active",                                "textarea:focus",
                "select:not([multiple])",      "select:not([multiple]):hover",                  "select:not([multiple]):active",                  "select:not([multiple]):focus",
                "select[multiple]",            "select[multiple]:hover",                        "select[multiple]:active",                        "select[multiple]:focus",
                "option",                      "option:hover",                                  "option:active",                                  "option:focus",
                ".list_box",                   ".list_box:hover",                               ".list_box:active",                               ".list_box:focus",
                ".list_box_item",              "._list_box_radio_buttons:hover+.list_box_item", "._list_box_radio_buttons:active+.list_box_item", "._list_box_radio_buttons:checked+.list_box_item",
                "table,th,td",                 "table:hover,th:hover,td:hover",                 "table:active,th:active,td:active",               "table:focus,th:focus,td:focus",
                "th",                          "th:hover",                                      "th:active",                                      "th:focus",
                "td",                          "td:hover",                                      "td:active",                                      "td:focus",
                "details summary",             "details summary:hover",                         "details summary:active",                         "details summary:focus",
                ".collapsing_header_body",     ".collapsing_header_body:hover",                 ".collapsing_header_body:active",                 ".collapsing_header_body:focus",
                ".child_window",               ".child_window:hover",                           ".child_window:active",                           ".child_window:focus",
                ".window_splitter_horizontal", ".window_splitter_horizontal:hover",             ".window_splitter_horizontal:active",             ".window_splitter_horizontal:focus",
                ".window_splitter_vertical",   ".window_splitter_vertical:hover",               ".window_splitter_vertical:active",               ".window_splitter_vertical:focus",
                ".menu",                       ".menu:hover",                                   ".menu:active",                                   ".menu:focus",
                ".menu_item",                  ".menu_item:hover",                              ".menu_item:active",                              ".menu_item:focus",
                ".menu_dropdown",              ".menu_dropdown:hover,",                         ".menu_dropdown:active",                          ".menu_dropdown:focus",
                ".menu_dropdown_item",         ".menu_dropdown_item:hover",                     ".menu_dropdown_item:active",                     ".menu_dropdown_item:focus"
            };
            auto constexpr event_type_strings = std::array<const char*, 101>{
                "mousedown",        "mouseup",                "mousemove",        "mouseenter",           "mouseleave",     
                "click",            "dblclick",               "contextmenu",      "dragstart",            "dragend",        
                "dragenter",        "dragleave",              "dragover",         "drop",                 "focusin",        
                "focusout",         "blur",                   "scroll",           "resize",               "load",           
                "unload",           "beforeunload",           "hashchange",       "popstate",             "pageshow",       
                "pagehide",         "error",                  "input",            "change",               "submit",         
                "reset",            "cut",                    "copy",             "paste",                "touchstart",     
                "touchmove",        "touchend",               "touchcancel",      "pointerdown",          "pointermove",    
                "pointerup",        "pointercancel",          "pointerenter",     "pointerleave",         "pointerover",    
                "pointerout",       "animationstart",         "animationend",     "animationiteration",   "transitionstart",
                "transitionend",    "transitionrun",          "transitioncancel", "compositionstart",     "compositionupdate",
                "compositionend",   "selectionstart",         "selectionend",     "selectionchange",      "progress",       
                "loadstart",        "loadend",                "loadeddata",       "loadedmetadata",       "timeupdate",     
                "play",             "pause",                  "ended",            "waiting",              "playing",        
                "durationchange",   "ratechange",             "volumechange",     "canplay",              "canplaythrough", 
                "seeking",          "seeked",                 "stalled",          "suspend",              "emptied",        
                "fullscreenchange", "resizeobserver",         "mutationobserver", "intersectionobserver", "websocketopen",
                "websocketclose",   "websocketmessage",       "websocketerror",   "contextlost",          "contextrestored",
                "vrdisplayconnect", "vrdisplaydisconnect",    "vrdisplayactivate","vrdisplaydeactivate",  "vrdisplayblur",
                "vrdisplayfocus",   "vrdisplaypresentchange", "keydown",          "keyup",                "keypress",
                "DOMContentLoaded"
            };
            auto constexpr border_type_strings = std::array<const char*, 10> {
                "solid", "double", "dashed", "dotted", "groove",
                "ridge", "inset",  "outset", "none",   "hidden"
            };
            template <range_of_type_or_convertible_or_toable<std::string> tp_range_t>
            web_nodiscard auto join_strings(tp_range_t&& p_range) -> std::string {
                return std::ranges::to<std::string>(std::views::join(p_range));
            }
            template <range_of_type_or_convertible_or_toable<std::string> tp_range_t, single_or_range_of<std::ranges::range_value_t<std::ranges::range_value_t<tp_range_t>>> tp_single_or_range_of_t>
            web_nodiscard auto join_strings_with(tp_range_t&& p_range, const tp_single_or_range_of_t& p_delimiter) -> std::string {
                //replace with std::views::join_with
                auto l_result = std::string{};
                for (auto&& a : p_range) {
                    l_result += std::ranges::to<std::string>(a);
                    l_result += p_delimiter;
                }
                if (!std::ranges::empty(p_range)) {
                    if constexpr (std::same_as<std::remove_cvref_t<tp_single_or_range_of_t>, std::ranges::range_value_t<std::ranges::range_value_t<tp_range_t>>>)
                        l_result.pop_back();
                    else l_result.resize(std::ranges::size(l_result) - std::ranges::size(p_delimiter));
                }
                return l_result;
            }
            template <std::integral tp_integral_t>
            web_nodiscard auto string_to_integer(const std::string& p_value) web_noexcept(true) -> tp_integral_t {
                auto l_result = tp_integral_t{0};
                std::from_chars(std::ranges::data(p_value), std::ranges::data(p_value) + std::ranges::size(p_value), l_result);
                return l_result; 
            }
        };
        
        enum class element_property_type            : std::uint8_t { inner_html, outer_html, inner_text, outer_text, text_content, value, checked, selected_options };
        enum class relativity_type                  : std::uint8_t { absolute, relative };
        enum class axis_type                        : std::uint8_t { horizontal, vertical };
        enum class horizontal_allignment            : std::uint8_t { left, center, right };
        enum class flex_vertical_allignment_type    : std::uint8_t { buttom, center, top };
        enum class non_flex_item_allignment_type    : std::uint8_t { stretch, flex_start, flex_end, center, baseline };
        enum class flex_display_type                : std::uint8_t { flex, inline_flex };
        enum class non_flex_display_type            : std::uint8_t { regular_inline, block, inline_block, grid, inline_grid };
        enum class flex_content_justification_type  : std::uint8_t { none, flex_start, flex_end, center, space_between, space_around, space_evenly, start, end, left, right };
        enum class non_flex_item_justification_type : std::uint8_t { none, normal, stretch, center, start, end, self_start, self_end, left, right, baseline, first_baseline, last_baseline, safe_center, unsafe_center };
        enum class overflow_encapsulation           : std::uint8_t { disabled, enabled };
        enum class wrap_type                        : std::uint8_t { disabled, enabled };
        enum class border_type                      : std::uint8_t { single_line, double_line, dashed, dotted, groove, ridge, inset, outset, none, hidden };
        enum class box_shadow_type                  : std::uint8_t { inner, outer };
        enum class style_key : std::uint8_t {
            text,                       text_hovered,                       text_active,                       text_focused,
            button,                     button_hovered,                     button_active,                     button_focused,
            input_text,                 input_text_hovered,                 input_text_active,                 input_text_focused,
            input_password,             input_password_hovered,             input_password_active,             input_password_focused,
            input_number,               input_number_hovered,               input_number_active,               input_number_focused,
            slider,                     slider_hovered,                     slider_active,                     slider_focused,
            search_box,                 search_box_hovered,                 search_box_active,                 search_box_focused,
            checkbox,                   checkbox_hovered,                   checkbox_active,                   checkbox_checked,
            radio_buttons,              radio_buttons_hovered,              radio_buttons_active,              radio_buttons_checked,
            text_box,                   text_box_hovered,                   text_box_active,                   text_box_focused,
            drop_down,                  drop_down_hovered,                  drop_down_active,                  drop_down_focused,
            selection,                  selection_hovered,                  selection_active,                  selection_focused,
            option,                     option_hovered,                     option_active,                     option_focused,
            list_box,                   list_box_hovered,                   list_box_active,                   list_box_focused,
            list_box_item,              list_box_item_hovered,              list_box_item_active,              list_box_item_focused,
            table,                      table_hovered,                      table_active,                      table_focused,
            table_header,               table_header_hovered,               table_header_active,               table_header_focused,
            table_body,                 table_body_hovered,                 table_body_active,                 table_body_focused,
            collapsing_header_header,   collapsing_header_header_hovered,   collapsing_header_header_active,   collapsing_header_header_focused,
            collapsing_header_body,     collapsing_header_body_hovered,     collapsing_header_body_active,     collapsing_header_body_focused,
            child_window,               child_window_hovered,               child_window_active,               child_window_focused,
            window_splitter_horizontal, window_splitter_horizontal_hovered, window_splitter_horizontal_active, window_splitter_horizontal_focused,
            window_splitter_vertical,   window_splitter_vertical_hovered,   window_splitter_vertical_active,   window_splitter_vertical_focused,
            menu,                       menu_hovered,                       menu_active,                       menu_focused,
            menu_item,                  menu_item_hovered,                  menu_item_active,                  menu_item_focused,
            menu_dropdown,              menu_dropdown_hovered,              menu_dropdown_active,              menu_dropdown_focused,
            menu_dropdown_item,         menu_dropdown_item_hovered,         menu_dropdown_item_active,         menu_dropdown_item_focused
        };
        enum class event_type : std::uint8_t {
            mouse_down,        mouse_up,                  mouse_move,         mouse_enter,           mouse_leave,      
            click,             double_click,              context_menu,       drag_start,            drag_end,         
            drag_enter,        drag_leave,                drag_over,          drag_drop,             focus_in,         
            focus_out,         blur,                      scroll,             resize,                load,            
            unload,            before_unload,             hash_change,        pop_state,             page_show,        
            page_hide,         error,                     input,              change,                submit,          
            reset,             cut,                       copy,               paste,                 touch_start,      
            touch_move,        touch_end,                 touch_cancel,       pointer_down,          pointer_move,     
            pointer_up,        pointer_cancel,            pointer_enter,      pointer_leave,         pointer_over,     
            pointer_out,       animation_start,           animation_end,      animation_iteration,   transition_start, 
            transition_end,    transition_run,            transition_cancel,  composition_start,     composition_update,
            composition_end,   selection_start,           selection_end,      selection_change,      progress,        
            load_start,        load_end,                  loaded_data,        loaded_metadata,       time_update,      
            play,              pause,                     ended,              waiting,               playing,         
            duration_change,   rate_change,               volume_change,      can_play,              can_play_through,  
            seeking,           seeked,                    stalled,            suspend,               emptied,         
            fullscreen_change, resize_observer,           mutation_observer,  intersection_observer, web_socket_open,  
            web_socket_close,  web_socket_message,        web_socket_error,   context_lost,          context_restored, 
            vrdisplay_connect, vrdisplay_disconnect,      vrdisplay_activate, vrdisplay_deactivate,  vrdisplay_blur,   
            vrdisplay_focus,   vrdisplay_present_changem, key_down,           key_up,                key_press,
            domcontent_loaded
        };

        namespace detail {
            template <auto tp_id>
            struct integral_wrap {
                std::uintmax_t m_value;
                template <std::integral tp_integral_type_t>
                web_nodiscard constexpr operator tp_integral_type_t() const web_noexcept(true) {
                    return static_cast<tp_integral_type_t>(m_value);
                }
                template <std::integral tp_integral_type_t>
                web_nodiscard auto constexpr operator=(const tp_integral_type_t p_value) web_noexcept(true) -> std::add_lvalue_reference_t<integral_wrap> {
                    return m_value = p_value;
                    return *this;
                }
            };
        }
        using integral_relative  = detail::integral_wrap<0>;
        using integral_absolute  = detail::integral_wrap<1>;
        using calculation_string = std::string;
        using colour_rgba        = std::array<std::uint8_t, 4>;
        using colour_hex         = std::size_t;

        struct style final {
            using optional_variant_of_sizing_types_t = std::optional<std::variant<integral_absolute, integral_relative, calculation_string>>;
            using optional_variant_of_colour_types_t = std::optional<std::variant<colour_hex, colour_rgba>>;
            std::optional<flex_vertical_allignment_type> vertical_allignment;
            std::optional<non_flex_item_allignment_type> item_allignment;
            optional_variant_of_sizing_types_t           width;
            optional_variant_of_sizing_types_t           height;
            optional_variant_of_sizing_types_t           margin;
            optional_variant_of_sizing_types_t           margin_horizontal;
            optional_variant_of_sizing_types_t           margin_vertical;
            optional_variant_of_sizing_types_t           margin_left;
            optional_variant_of_sizing_types_t           margin_top;
            optional_variant_of_sizing_types_t           margin_right;
            optional_variant_of_sizing_types_t           margin_bottom;
            optional_variant_of_sizing_types_t           padding;
            optional_variant_of_sizing_types_t           padding_horizontal;
            optional_variant_of_sizing_types_t           padding_vertical;
            optional_variant_of_sizing_types_t           padding_left;
            optional_variant_of_sizing_types_t           padding_top;
            optional_variant_of_sizing_types_t           padding_right;
            optional_variant_of_sizing_types_t           padding_bottom;
            std::optional<std::uintmax_t>                border_thickness;
            std::optional<std::uintmax_t>                border_rounding; //should be 4 values?
            std::optional<border_type>                   border_type;
            //std::optional<std::uintmax_t>                box_shadow_thickness;
            //std::optional<box_shadow_type>               box_shadow_type;
            std::optional<std::string>                   font;
            std::optional<std::string>                   font_weight;
            std::optional<std::uintmax_t>                font_size;
            std::optional<std::string>                   cursor;
            optional_variant_of_colour_types_t           colour;
            optional_variant_of_colour_types_t           background_colour;
            optional_variant_of_colour_types_t           border_colour;
            //optional_variant_of_colour_types_t           box_shadow_colour;

            web_nodiscard auto get_properties_as_string() const -> std::string {
                auto constexpr static l_conditionally_add_property = [](this auto p_self, auto&& p_output, auto&& p_property_name, const bool p_should_enclose_property_value_in_single_quote, auto&& p_optional_variant) {
                    using optional_variant_type_t = std::remove_cvref_t<decltype(p_optional_variant)>;
                    if (p_optional_variant.has_value()) {
                        auto l_result = [](auto&& p_property_name, const bool p_should_enclose_property_value_in_single_quote, auto&& p_optional_variant) {
                            if constexpr (std::same_as<optional_variant_type_t, optional_variant_of_sizing_types_t>) {
                                if (auto l_calculation_string_unwrapped = std::get_if<calculation_string>(std::addressof(p_optional_variant.value())))
                                    return std::format("{}:calc({});", p_property_name, *l_calculation_string_unwrapped);
                                else if (auto l_unwrapped_integral_absolute = std::get_if<integral_absolute>(std::addressof(p_optional_variant.value())))
                                    return std::format("{}:{}{};", p_property_name, static_cast<std::uintmax_t>(*l_unwrapped_integral_absolute), "px");
                                else return std::format("{}:{}{};", p_property_name, static_cast<std::uintmax_t>(std::get<integral_relative>(p_optional_variant.value())), '%');
                            }
                            else if constexpr (std::same_as<optional_variant_type_t, optional_variant_of_colour_types_t>) {
                                if (auto l_colour_rgba_unwrapped = std::get_if<colour_rgba>(std::addressof(p_optional_variant.value())))
                                    return std::format("{}:rgba({},{},{},{});", p_property_name, (*l_colour_rgba_unwrapped)[0], (*l_colour_rgba_unwrapped)[1], (*l_colour_rgba_unwrapped)[2], (*l_colour_rgba_unwrapped)[3]);
                                else return std::format("{}:#{};", p_property_name, std::get<colour_hex>(p_optional_variant.value()));
                            }
                            else if constexpr (std::same_as<optional_variant_type_t, std::optional<std::string>>)
                                return std::format("{}:{};", p_property_name, !p_should_enclose_property_value_in_single_quote ? p_optional_variant.value() : std::format("{}{}{}", '\'', p_optional_variant.value(), '\''));
                            else return std::format("{}:{}px;", p_property_name, p_optional_variant.value());
                        }(p_property_name, p_should_enclose_property_value_in_single_quote, p_optional_variant);
                        p_output += l_result;
                    }
                    return p_self;
                };
                auto constexpr static l_conditionally_add_enum_property = [](this auto p_self, auto&& p_output, auto&& p_property_name, auto&& p_property_enum_value, auto&& p_enum_string_map) {
                    if (p_property_enum_value.has_value())
                        std::ranges::copy(std::format("{}:{};", p_property_name, p_enum_string_map[std::to_underlying(p_property_enum_value.value())]), std::back_inserter(p_output));
                    return p_self;
                };
                auto l_result = std::string{};
                l_conditionally_add_enum_property
                    (l_result, "vertical-align", vertical_allignment, detail::flex_vertical_allignment_type_strings)
                    (l_result, "align-items",    item_allignment,     detail::non_flex_item_allignment_type_strings)
                    (l_result, "border-style",   border_type,         detail::border_type_strings);
                l_conditionally_add_property
                    (l_result, "width",            false, width)
                    (l_result, "height",           false, height)
                    (l_result, "margin",           false, margin)
                    (l_result, "margin-left",      false, margin_horizontal.has_value() ? margin_horizontal : margin_left)
                    (l_result, "margin-top",       false, margin_vertical.has_value()   ? margin_vertical   : margin_top)
                    (l_result, "margin-right",     false, margin_horizontal.has_value() ? margin_horizontal : margin_right)
                    (l_result, "margin-bottom",    false, margin_vertical.has_value()   ? margin_vertical   : margin_bottom)
                    (l_result, "padding",          false, padding)
                    (l_result, "padding-left",     false, padding_horizontal.has_value() ? padding_horizontal : padding_left)
                    (l_result, "padding-top",      false, padding_vertical.has_value()   ? padding_vertical   : padding_top)
                    (l_result, "padding-right",    false, padding_horizontal.has_value() ? padding_horizontal : padding_right)
                    (l_result, "padding-bottom",   false, padding_vertical.has_value()   ? padding_vertical   : padding_top)
                    (l_result, "border-width",     false, border_thickness)
                    (l_result, "border-radius",    false, border_rounding)
                    (l_result, "font-family",      true,  font)
                    (l_result, "font-weight",      false, font_weight)
                    (l_result, "font-size",        false, font_size)
                    (l_result, "cursor",           false, cursor)
                    (l_result, "color",            false, colour)
                    (l_result, "background-color", false, background_colour)
                    (l_result, "border-color",     false, border_colour);
                //if (box_shadow_thickness.has_value() || box_shadow_type.has_value() || box_shadow_colour.has_value()) {
                //    l_result += "box-shadow:";
                //    if (box_shadow_type.has_value() && box_shadow_type == box_shadow_type::inner)
                //        l_result += "inset";
                //    if (box_shadow_thickness.has_value())
                //        l_result += std::format(" {}", box_shadow_thickness.value());
                //    if (box_shadow_colour.has_value())
                //        if (auto l_colour_rgba_unwrapped = std::get_if<colour_rgba>(std::addressof(box_shadow_colour.value())))
                //            l_result += std::format(" rgba({},{},{},{})", (*l_colour_rgba_unwrapped)[0], (*l_colour_rgba_unwrapped)[1], (*l_colour_rgba_unwrapped)[2], (*l_colour_rgba_unwrapped)[3]);
                //        else l_result += std::format(" #{}", std::get<colour_hex>(box_shadow_colour.value()));
                //    l_result += ';';
                //}
                return l_result;
            };
            web_nodiscard auto to_inline_string() const -> std::string {
                return std::format(R"(style="{}")", get_properties_as_string());
            }
            web_nodiscard auto to_external_string() const -> std::string {
                return std::format("*{{{}}}", get_properties_as_string());
            }
            web_nodiscard auto to_external_string(const style_key p_style_key) const -> std::string {
                return std::format("{}{{{}}}", detail::style_key_strings[std::to_underlying(p_style_key)], get_properties_as_string());
            }
            template <detail::range_of<style_key> tp_style_key_range_t = std::initializer_list<style_key>>
            web_nodiscard auto to_external_string(tp_style_key_range_t&& p_style_keys) const -> std::string {
                return std::format("{}{{{}}}", std::ranges::empty(p_style_keys) ? "*"sv : std::string_view{detail::join_strings_with(std::views::transform(p_style_keys, [](auto&& a) { return std::string_view{detail::style_key_strings[std::to_underlying(a)]}; }), ',')}, get_properties_as_string()); //don't need to construct string_view when changing to std::views::join_with"
            }
        };

        namespace detail {
            auto callback_function_map                  = std::unordered_map<std::string, std::unordered_map<std::string, std::function<void(void)>>>{};
            auto registered_elements                    = std::vector<std::string>{};
            auto registered_scripts                     = std::vector<std::function<void(void)>>{};
            auto registered_on_initialization_callables = std::vector<std::function<void(void)>>{};
            auto external_styles                        = std::vector<std::string>{};
            extern "C" {
                EMSCRIPTEN_KEEPALIVE auto cxx_callback_invoker(const char* p_id, const char* p_event_type_string) -> void {
                    callback_function_map[p_id][p_event_type_string]();
                }
            }
            template <can_construct<std::string> tp_string_constructible_type_t>
            auto register_element(tp_string_constructible_type_t&& p_value) -> void {
                registered_elements.emplace_back(std::forward<tp_string_constructible_type_t>(p_value));
            }
            auto add_element(const std::string& p_element_html) -> void {
                std::cout << p_element_html << std::endl;
            }
            template <can_construct<std::function<void(void)>> tp_stdfunction_constructible_type_t>
            auto register_event_listener(const std::string& p_id, const event_type p_event_type, tp_stdfunction_constructible_type_t&& p_callable) -> void {
                if (!detail::callback_function_map.contains(p_id))
                    detail::callback_function_map.emplace(p_id, typename std::ranges::range_value_t<decltype(detail::callback_function_map)>::second_type{});
                detail::callback_function_map.at(p_id).emplace(detail::event_type_strings[std::to_underlying(p_event_type)], std::forward<tp_stdfunction_constructible_type_t>(p_callable));
            }
            auto add_event_listener(const std::string_view& p_element_id, const std::string_view& p_listener_type) -> void {
                EM_ASM((
                    document.getElementById(UTF8ToString($0)).addEventListener(UTF8ToString($1), function() { ccall("cxx_callback_invoker", null, ["string", "string"], [UTF8ToString($0), UTF8ToString($1)
                    ]); });
                ), std::ranges::data(p_element_id), std::ranges::data(p_listener_type));
            }
            template <can_construct<std::function<void(void)>> tp_stdfunction_constructible_type_t>
            auto register_script(tp_stdfunction_constructible_type_t&& p_callable) -> void {
                registered_scripts.emplace_back(std::forward<tp_stdfunction_constructible_type_t>(p_callable));
            }
            template <can_construct<std::string> tp_string_constructible_type1_t = std::string_view, can_construct<std::string> tp_string_constructible_type2_t = std::string_view, can_construct<std::string> tp_string_constructible_type3_t = std::string_view, can_construct<std::string> tp_string_constructible_type4_t = std::string_view>
            web_nodiscard auto make_element(tp_string_constructible_type1_t&& p_tag, tp_string_constructible_type2_t&& p_arguments, tp_string_constructible_type3_t&& p_id, const std::optional<std::string>& p_label_text, const std::optional<std::size_t>& p_label_size, const std::optional<style>& p_inline_style, tp_string_constructible_type4_t&& p_body = {}) -> std::string {
                return std::format(
                    R"({}<{} {} id="{}" {}>{}</{}>)",
                    !p_label_text.has_value() ? ""sv : std::string_view{std::format(R"(<label for="{}" {}>{}</label>)", p_id, std::format(R"(style="display:inline-block;{}")", !p_label_size.has_value() ? std::string_view{} : std::string_view{std::format("width:{}px;", p_label_size.value())}), p_label_text.value())},
                    p_tag, p_arguments, p_id, !p_inline_style.has_value() ? std::string_view{} : std::string_view{p_inline_style.value().to_inline_string()}, p_body, p_tag);
            }
        };
        
        namespace detail {
            struct on_initialize_t {
                template <can_construct<std::function<void(void)>> tp_stdfunction_constructible_type_t>
                web_mayunused auto operator()(tp_stdfunction_constructible_type_t&& p_callable) const -> on_initialize_t {
                    detail::registered_on_initialization_callables.emplace_back(std::forward<tp_stdfunction_constructible_type_t>(p_callable));
                    return *this;
                }
            };
        }
        auto constexpr on_initialize = detail::on_initialize_t{};

        namespace detail {
            struct finalize_t {
                web_mayunused auto operator()() const -> finalize_t {
                    std::cout <<
                        "<style>" << detail::join_strings(detail::external_styles) << "</style>" <<
                        detail::join_strings(detail::registered_elements)
                    << std::endl;
                    std::ranges::for_each(detail::registered_scripts, [](auto&& a) { a(); });
                    for (auto&& a : detail::callback_function_map)
                        for (auto&& b : a.second)
                            detail::add_event_listener(a.first, b.first);
                    std::ranges::for_each(detail::registered_on_initialization_callables, [](auto&& a) { a(); });
                    return *this;
                }
            };
        }
        auto finalize = detail::finalize_t{};

        namespace detail {
            struct set_callback_t {
                template <can_construct<std::function<void(void)>> tp_stdfunction_constructible_type_t>
                web_mayunused auto operator()(const std::string& p_id, const event_type p_event_type, tp_stdfunction_constructible_type_t&& p_callable) const -> set_callback_t {
                    detail::register_event_listener(p_id, p_event_type, std::forward<tp_stdfunction_constructible_type_t>(p_callable));
                    return *this;
                }
            };
        }
        auto set_callback = detail::set_callback_t{};

        namespace detail {
            struct set_style_t {
                web_mayunused auto operator()(const style& p_style) const -> set_style_t {
                    detail::external_styles.emplace_back(p_style.to_external_string());
                    return *this;
                }
                web_mayunused auto operator()(const style_key p_style_key, const style& p_style) const -> set_style_t {
                    detail::external_styles.emplace_back(p_style.to_external_string(p_style_key));
                    return *this;
                }
                template <detail::range_of<style_key> tp_style_key_range_t = std::initializer_list<style_key>>
                web_mayunused auto operator()(tp_style_key_range_t&& p_style_key_range, const style& p_style) const -> set_style_t {
                    detail::external_styles.emplace_back(p_style.to_external_string(p_style_key_range));
                    return *this;
                }
            };
        }
        auto constexpr set_style = detail::set_style_t{};
        
        namespace detail {
            template <element_property_type tp_element_property_type>
            struct set_element_property_t {
                web_mayunused auto operator()(const std::string& p_element_id, const std::string& p_value) const -> set_element_property_t {
                    if constexpr (tp_element_property_type == element_property_type::inner_html)
                        EM_ASM(( document.getElementById(UTF8ToString($0)).innerHTML = UTF8ToString($1); ), p_element_id.c_str(), p_value.c_str());
                    else if constexpr (tp_element_property_type == element_property_type::outer_html)
                        EM_ASM(( document.getElementById(UTF8ToString($0)).outerHTML = UTF8ToString($1); ), p_element_id.c_str(), p_value.c_str());
                    else if constexpr (tp_element_property_type == element_property_type::inner_text)
                        EM_ASM(( document.getElementById(UTF8ToString($0)).innerText = UTF8ToString($1); ), p_element_id.c_str(), p_value.c_str());
                    else if constexpr (tp_element_property_type == element_property_type::outer_text)
                        EM_ASM(( document.getElementById(UTF8ToString($0)).outerText = UTF8ToString($1); ), p_element_id.c_str(), p_value.c_str());
                    else if constexpr (tp_element_property_type == element_property_type::text_content)
                        EM_ASM(( document.getElementById(UTF8ToString($0)).textContent = UTF8ToString($1); ), p_element_id.c_str(), p_value.c_str());
                    else if constexpr (tp_element_property_type == element_property_type::value)
                        EM_ASM(( document.getElementById(UTF8ToString($0)).value = UTF8ToString($1); ), p_element_id.c_str(), p_value.c_str());
                    return *this;
                }
            };
        }
        template <element_property_type tp_element_property_type>
        auto constexpr set_element_property = detail::set_element_property_t<tp_element_property_type>{};

    //add extra index paramter here, for targetting sub elements
        namespace detail {
            template <element_property_type tp_element_property_type>
            struct get_element_property_t {
                web_mayunused auto operator()(const std::string& p_element_id) const -> std::string {
                    auto l_char_pointer = std::add_pointer_t<char>{nullptr};
                    if constexpr (tp_element_property_type == element_property_type::inner_html)
                        l_char_pointer = static_cast<char*>(EM_ASM_PTR(( return stringToNewUTF8(document.getElementById(UTF8ToString($0)).innerHTML); ), p_element_id.c_str()));
                    else if constexpr (tp_element_property_type == element_property_type::outer_html)
                        l_char_pointer = static_cast<char*>(EM_ASM_PTR(( return stringToNewUTF8(document.getElementById(UTF8ToString($0)).outerHTML); ), p_element_id.c_str()));
                    else if constexpr (tp_element_property_type == element_property_type::inner_text)
                        l_char_pointer = static_cast<char*>(EM_ASM_PTR(( return stringToNewUTF8(document.getElementById(UTF8ToString($0)).innerText); ), p_element_id.c_str()));
                    else if constexpr (tp_element_property_type == element_property_type::outer_text)
                        l_char_pointer = static_cast<char*>(EM_ASM_PTR(( return stringToNewUTF8(document.getElementById(UTF8ToString($0)).outerText); ), p_element_id.c_str()));
                    else if constexpr (tp_element_property_type == element_property_type::text_content)
                        l_char_pointer = static_cast<char*>(EM_ASM_PTR(( return stringToNewUTF8(document.getElementById(UTF8ToString($0)).textContent); ), p_element_id.c_str()));
                    else if constexpr (tp_element_property_type == element_property_type::value)
                        l_char_pointer = static_cast<char*>(EM_ASM_PTR(( return stringToNewUTF8(document.getElementById(UTF8ToString($0)).value); ), p_element_id.c_str()));
                    else if constexpr (tp_element_property_type == element_property_type::checked)
                        l_char_pointer = static_cast<char*>(EM_ASM_PTR(( return stringToNewUTF8(document.getElementById(UTF8ToString($0)).checked.toString()); ), p_element_id.c_str()));
                    else if constexpr (tp_element_property_type == element_property_type::selected_options)
                        l_char_pointer = static_cast<char*>(EM_ASM_PTR(( return stringToNewUTF8(Array.from(document.getElementById(UTF8ToString($0)).selectedOptions).map(option => option.value).join(', ')); ), p_element_id.c_str()));
                    auto l_result = std::string{l_char_pointer};
                    free(l_char_pointer);
                    return l_result;
                }
            };
        }
        template <element_property_type tp_element_property_type>
        auto constexpr get_element_property = detail::get_element_property_t<tp_element_property_type>{};
        
        namespace detail {
            struct show_element_t {
                //ResetElementStyle, Set/GetElementStyle? //element.style.cssText = ""
                web_mayunused auto operator()(const std::string_view& p_id) const -> show_element_t{
                    EM_ASM(( document.getElementById(UTF8ToString($0)).style.display = "inline-block"; ), std::ranges::data(p_id));
                    return *this;
                }
            };
        }
        auto constexpr show_element = detail::show_element_t{};

        namespace detail {
            struct hide_element_t {
                web_mayunused auto operator()(const std::string& p_id) const -> hide_element_t {
                    EM_ASM(( document.getElementById(UTF8ToString($0)).style.display = "none"; ), p_id.c_str());
                    return *this;
                }
            };
        }
        auto constexpr hide_element = detail::hide_element_t{};

        namespace detail {
            struct set_scroll_to_top_t {
                web_mayunused auto operator()(const std::string_view& p_id) const -> set_scroll_to_top_t {
                    EM_ASM(( document.getElementById(UTF8ToString($0)).scrollTop = 0; ), std::ranges::data(p_id));
                    return *this;
                }
            };
        }
        auto constexpr set_scroll_to_top = detail::set_scroll_to_top_t{};

        namespace detail {
            struct set_scroll_to_bottom_t {
                web_mayunused auto operator()(const std::string_view& p_id) const -> set_scroll_to_bottom_t {
                    EM_ASM((
                        var window = document.getElementById(UTF8ToString($0));
                        window.scrollTop = window.scrollHeight;
                    ), std::ranges::data(p_id));
                    return *this;
                }
            };
        }
        auto constexpr set_scroll_to_bottom = detail::set_scroll_to_bottom_t{};
        
        namespace detail {
            template <axis_type tp_axis_type>
            struct get_scroll_t {
                web_nodiscard auto operator()(const std::string_view& p_id) const -> std::uintmax_t {
                    return static_cast<std::uintmax_t>(
                        tp_axis_type == axis_type::vertical ?
                            EM_ASM_INT(( document.getElementById(UTF8ToString($0)).scrollTop; ), std::ranges::data(p_id)) :
                            EM_ASM_INT(( document.getElementById(UTF8ToString($0)).scrollLeft; ), std::ranges::data(p_id))
                    );
                }
            };
        }
        template <axis_type tp_axis_type>
        auto constexpr get_scroll = detail::get_scroll_t<tp_axis_type>{};

        namespace detail {
            template <axis_type tp_axis_type>
            struct set_scroll_t {
                web_mayunused auto operator()(const std::string_view& p_id, const std::size_t p_value) const -> set_scroll_t {
                    if constexpr (tp_axis_type == axis_type::horizontal)
                        EM_ASM(( document.getElementById(UTF8ToString($0)).scrollTop = $1; ), std::ranges::data(p_id), p_value);
                    else EM_ASM(( document.getElementById(UTF8ToString($0)).scrollLeft = $1; ), std::ranges::data(p_id), p_value);
                    return *this;
                }
            };
        }
        template <axis_type tp_axis_type>
        auto constexpr set_scroll = detail::set_scroll_t<tp_axis_type>{};

        namespace detail {
            template <bool tp_new_tab>
            struct go_to_url_t {
                web_mayunused auto operator()(const std::string_view& p_url) const -> go_to_url_t {
                    if constexpr (tp_new_tab)
                        EM_ASM(( window.open(UTF8ToString($0), '_blank'); ), std::ranges::data(p_url));
                    else EM_ASM(( window.location.href = UTF8ToString($0); ), std::ranges::data(p_url));
                    return *this;
                }
            };
        }
        template <bool tp_new_tab>
        auto constexpr go_to_url = detail::go_to_url_t<tp_new_tab>{};

        namespace detail {
            struct get_table_row_count_t {
                web_nodiscard auto operator()(const std::string_view& p_id) const -> std::uintmax_t {
                    return static_cast<std::uintmax_t>(EM_ASM_INT(( return document.getElementById(UTF8ToString($0)).getElementsByTagName('tbody')[0].rows.length; ), std::ranges::data(p_id)));
                }
            };
        }
        auto constexpr get_table_row_count = detail::get_table_row_count_t{};

        namespace detail {
            struct add_table_element_t {
                template <detail::range_of<std::string> tp_string_range_t = std::initializer_list<std::string>>
                web_mayunused auto operator()(const std::string_view& p_id, const std::size_t p_index, tp_string_range_t&& p_data) const -> add_table_element_t {
                    EM_ASM(( document.getElementById(UTF8ToString($0)).getElementsByTagName('tbody')[0].insertRow($1); ), std::ranges::data(p_id), p_index);
                    for (auto&& a : p_data)
                        EM_ASM(( document.getElementById(UTF8ToString($0)).getElementsByTagName('tbody')[0].rows[$1].insertCell().innerHTML = UTF8ToString($2); ), std::ranges::data(p_id), p_index, a.c_str());
                    return *this;
                }
            };
        }
        auto constexpr add_table_element = detail::add_table_element_t{};

        namespace detail {
            struct remove_table_element_t {
                web_mayunused auto operator()(const std::string_view& p_id, const std::size_t p_index) const -> remove_table_element_t {
                    EM_ASM((
                        var table = document.getElementById(UTF8ToString($0)).getElementsByTagName('tbody')[0];
                        var rowCount = table.rows.length;
                        if (rowCount > 1) {
                            table.deleteRow($1);
                        }
                    ), std::ranges::data(p_id), p_index);
                    return *this;
                }
            };
        }
        auto constexpr remove_table_element = detail::remove_table_element_t{};
        
        //add only whole words mode?
        namespace detail {
            struct filter_table_t {
                auto operator()(const std::string_view& p_id, const std::string_view& p_value, const bool p_contains, const bool p_case_sensitive, const bool p_skip_first_row) const -> filter_table_t {
                    EM_ASM({
                        const rows = Array.from(document.getElementById(UTF8ToString($0)).querySelectorAll("tr"));
                        rows.forEach((row, index) => {
                            if ($4 && index === 0) return;
                            const includesText = Array.from(row.querySelectorAll("td")).some(cell => {
                                const searchText = ($3 ? UTF8ToString($1) : UTF8ToString($1).toLowerCase());
                                const cellText = ($3 ? cell.textContent : cell.textContent.toLowerCase());
                                return cellText.includes(searchText);
                            });
                            row.style.display = (($2 && includesText) || (!$2 && !includesText)) ? "none" : "table-row";
                        });
                    }, std::ranges::data(p_id), std::ranges::data(p_value), !p_contains, p_case_sensitive, p_skip_first_row);
                    return *this;
                }
            };
        }
        auto constexpr filter_table = detail::filter_table_t{};

        namespace detail {
            struct get_selected_list_box_item_t {
                web_mayunused auto operator()(const std::string_view& p_id) const -> std::string {
                    auto l_char_pointer = std::add_pointer_t<char>{nullptr};
                    l_char_pointer = static_cast<char*>(EM_ASM_PTR(( return stringToNewUTF8(document.querySelector('input[name="' + UTF8ToString($0) + '"]:checked').value); ), std::ranges::data(p_id)));
                    auto l_result = std::string{l_char_pointer};
                    free(l_char_pointer);
                    return l_result;
                }
            };
        }
        auto constexpr get_selected_list_box_item = detail::get_selected_list_box_item_t{};

        namespace detail {
            struct set_selected_list_box_item_t {
                web_mayunused auto operator()(const std::string_view& p_id, const std::size_t p_index) const -> set_selected_list_box_item_t {
                    EM_ASM(( document.getElementsByName(UTF8ToString($0))[$1].checked = true; ), std::ranges::data(p_id), p_index);
                    return *this;
                }
                web_mayunused auto operator()(const std::string_view& p_list_box_id, const std::string_view& p_item_id) const -> set_selected_list_box_item_t {
                    EM_ASM(( document.getElementById(UTF8ToString($0)).checked = true; ), std::format("{}_{}", p_list_box_id, p_item_id).c_str());
                    return *this;
                }
            };
        }
        auto constexpr set_selected_list_box_item = detail::set_selected_list_box_item_t{};

        namespace detail {
            struct new_line_t {
                web_mayunused auto operator()() const -> new_line_t {
                    detail::register_element("<br>");
                    return *this;
                }
            };
        }
        auto constexpr new_line = detail::new_line_t{};

    //add label
        namespace detail {
            struct text_t {
                template <can_construct<std::string> tp_string_constructible_type1_t, can_construct<std::string> tp_string_constructible_type2_t>
                web_mayunused auto operator()(tp_string_constructible_type1_t&& p_id, tp_string_constructible_type2_t&& p_value, const std::optional<style>& p_inline_style = {}) const -> text_t {
                    detail::register_element(std::string("<style>span{display:inline-block;}</style>") + detail::make_element("span", {}, std::forward<tp_string_constructible_type1_t>(p_id), {}, {}, p_inline_style, std::forward<tp_string_constructible_type2_t>(p_value)));
                    return *this;
                }
            };
        }
        auto constexpr text = detail::text_t{};

    //add label
        namespace detail {
            struct link_t {
                template <can_construct<std::string> tp_string_constructible_type1_t, can_construct<std::string> tp_string_constructible_type2_t>
                web_mayunused auto operator()(tp_string_constructible_type1_t&& p_id, tp_string_constructible_type2_t&& p_link) const -> link_t {
                    detail::register_element(detail::make_element("a", std::format(R"(href="{}")", p_link), std::forward<tp_string_constructible_type1_t>(p_id), {}, {}, {}, std::forward<tp_string_constructible_type2_t>(p_link)));
                    return *this;
                }
            };
        }
        auto constexpr link = detail::link_t{};

    //add label
        namespace detail {
            struct image_t {
                template <can_construct<std::string> tp_string_constructible_type1_t, can_construct<std::string> tp_string_constructible_type2_t>
                web_mayunused auto operator()(tp_string_constructible_type1_t&& p_id, tp_string_constructible_type2_t&& p_url, const std::optional<std::string>& p_alternative_text = {}, const std::optional<style>& p_inline_style = {}) const -> image_t{
                    detail::register_element(std::format(R"(<img id="{}" src="{}" alt="{}" {}>)",
                        p_id, p_url, !p_alternative_text.has_value() ? std::string_view{} : std::string_view{std::format(R"(alt="{}")", p_alternative_text.value())},
                        !p_inline_style.has_value() ? std::string_view{} : std::string_view{p_inline_style.value().to_inline_string()}
                    ));
                    return *this;
                }
            };
        }
        auto constexpr image = detail::image_t{};

    //add label
        namespace detail {
            struct image_button_t {
                template <can_construct<std::string> tp_string_constructible_type1_t, can_construct<std::string> tp_string_constructible_type2_t, can_construct<std::function<void(void)>> tp_stdfunction_constructible_type_t>
                web_mayunused auto operator()(tp_string_constructible_type1_t&& p_id, tp_string_constructible_type2_t&& p_url, tp_stdfunction_constructible_type_t&& p_on_click_callable, const std::optional<std::string>& p_alternative_text = {}, const std::optional<style>& p_inline_style = {}) const -> image_button_t {
                    detail::register_event_listener(p_id, event_type::click, std::forward<tp_stdfunction_constructible_type_t>(p_on_click_callable));
                    image(std::forward<tp_string_constructible_type1_t>(p_id), std::forward<tp_string_constructible_type2_t>(p_url), p_alternative_text, p_inline_style);
                    return *this;
                }
            };
        }
        auto constexpr image_button = detail::image_button_t{};

        //seperator
        //input file
        //input colour
        //input image
        //main popup window/modal
        //radio

        namespace detail {
            struct button_t {
                template <can_construct<std::string> tp_string_constructible_type1_t, can_construct<std::string> tp_string_constructible_type2_t, can_construct<std::function<void(void)>> tp_stdfunction_constructible_type_t>
                web_mayunused auto operator()(tp_string_constructible_type1_t&& p_id, tp_string_constructible_type2_t&& p_value, tp_stdfunction_constructible_type_t&& p_on_click_callable, const std::optional<std::string>& p_label_text = {}, const std::optional<std::size_t>& p_label_size = {}, const std::optional<style>& p_inline_style = {}) const -> button_t {
                    detail::register_event_listener(std::string{p_id}, event_type::click, std::forward<tp_stdfunction_constructible_type_t>(p_on_click_callable));
                    detail::register_element(detail::make_element("button", {}, std::forward<tp_string_constructible_type1_t>(p_id), p_label_text, p_label_size, p_inline_style, std::forward<tp_string_constructible_type2_t>(p_value)));
                    return *this;
                }
            };
        }
        auto constexpr button = detail::button_t{};
        
        namespace detail {
            template <can_construct<std::string> tp_string_constructible_type_t>
            web_mayunused auto input_impl(const char* p_type, tp_string_constructible_type_t&& p_id, const std::optional<std::string>& p_placeholder_text = {}, const std::optional<std::string>& p_label_text = {}, const std::optional<std::size_t>& p_label_size = {}, const std::optional<style>& p_inline_style = {}) -> void {
                detail::register_element(detail::make_element("input", std::format(R"(type="{}"{})", p_type, !p_placeholder_text.has_value() ? std::string_view{} : std::string_view{std::format(R"( placeholder="{}")", p_placeholder_text.value())}), std::forward<tp_string_constructible_type_t>(p_id), p_label_text, p_label_size, p_inline_style));
            }        
        }

        namespace detail {
            struct input_text_t {
                template <can_construct<std::string> tp_string_constructible_type_t>
                web_mayunused auto operator()(tp_string_constructible_type_t&& p_id, const std::optional<std::string>& p_placeholder_text = {}, const std::optional<std::string>& p_label_text = {}, const std::optional<std::size_t>& p_label_size = {}, const std::optional<style>& p_inline_style = {}) const -> input_text_t {
                    input_impl("text", std::forward<tp_string_constructible_type_t>(p_id), p_placeholder_text, p_label_text, p_label_size, p_inline_style);
                    return *this;
                }
            };
        }
        auto constexpr input_text = detail::input_text_t{};

        namespace detail {
            struct input_password_t {
                template <can_construct<std::string> tp_string_constructible_type_t>
                web_mayunused auto operator()(tp_string_constructible_type_t&& p_id, const std::optional<std::string>& p_placeholder_text = {}, const std::optional<std::string>& p_label_text = {}, const std::optional<std::size_t>& p_label_size = {}, const std::optional<style>& p_inline_style = {}) const -> input_password_t {
                    input_impl("password", std::forward<tp_string_constructible_type_t>(p_id), p_placeholder_text, p_label_text, p_label_size, p_inline_style);
                    return *this;
                }    
            };
        }
        auto constexpr input_password = detail::input_password_t{};

        namespace detail {
            struct input_number_t {
                template <can_construct<std::string> tp_string_constructible_type_t>
                web_mayunused auto operator()(tp_string_constructible_type_t&& p_id, const std::optional<std::string>& p_placeholder_text = {}, const std::optional<std::string>& p_label_text = {}, const std::optional<std::size_t>& p_label_size = {}, const std::optional<style>& p_inline_style = {}) const -> input_number_t {
                    input_impl("number", std::forward<tp_string_constructible_type_t>(p_id), p_placeholder_text, p_label_text, p_label_size, p_inline_style);
                    return *this;
                }    
            };
        }
        auto constexpr input_number = detail::input_number_t{};

        namespace detail {
            struct slider_t {
                template <can_construct<std::string> tp_string_constructible_type_t>
                web_mayunused auto operator()(tp_string_constructible_type_t&& p_id, const std::intmax_t p_min, const std::intmax_t p_max, const std::intmax_t p_initial_value, std::optional<std::function<void(std::intmax_t)>> p_on_drag_callable = {}, const std::optional<std::string>& p_label_text = {}, const std::optional<std::size_t>& p_label_size = {}, const std::optional<style>& p_inline_style = {}) const -> slider_t {
                    if (p_on_drag_callable.has_value())
                        detail::register_event_listener(p_id, event_type::change, [c_id = std::string{p_id}, c_on_drag_callable = p_on_drag_callable.value()]{ c_on_drag_callable(string_to_integer<std::intmax_t>(get_element_property<element_property_type::value>(c_id))); });
                    detail::register_element(detail::make_element("input", std::format(R"(type="range" min="{}" max="{}" value={})", p_min, p_max, p_initial_value), std::forward<tp_string_constructible_type_t>(p_id), p_label_text, p_label_size, p_inline_style)); //replace with std::format
                    return *this;
                }
            };
        }
        auto constexpr slider = detail::slider_t{};

        namespace detail {
            struct search_box_t {
                template <can_construct<std::string> tp_string_constructible_type_t, can_construct<std::function<void(std::string)>> tp_stdfunction_constructible_type_t>
                web_mayunused auto operator()(tp_string_constructible_type_t&& p_id, tp_stdfunction_constructible_type_t&& p_on_char_input_callable, const std::optional<std::string>& p_placeholder_text = {}, const std::optional<std::string>& p_label_text = {}, const std::optional<std::size_t>& p_label_size = {}, const std::optional<style>& p_inline_style = {}) const -> search_box_t {
                    detail::register_event_listener(p_id, event_type::input, [c_id = std::string{p_id}, c_on_char_input_callable = p_on_char_input_callable]{ c_on_char_input_callable(get_element_property<element_property_type::value>(c_id)); });
                    input_impl("search", std::forward<tp_string_constructible_type_t>(p_id), p_placeholder_text, p_label_text, p_label_size, p_inline_style);
                    return *this;
                }
            };
        }
        auto constexpr search_box = detail::search_box_t{};

        namespace detail {
            struct checkbox_t {
                template <can_construct<std::string> tp_string_constructible_type_t>
                web_mayunused auto operator()(tp_string_constructible_type_t&& p_id, const bool p_initial_value, const std::optional<std::function<void(bool)>>& p_on_toggle_callable = {}, const std::optional<std::string>& p_label_text = {}, const std::optional<std::size_t>& p_label_size = {}, const std::optional<style>& p_inline_style = {}) const -> checkbox_t {
                    if (p_on_toggle_callable.has_value())
                        detail::register_event_listener(p_id, event_type::click, [c_id = std::string{p_id}, c_on_toggle_callable = p_on_toggle_callable.value()]{ c_on_toggle_callable(get_element_property<element_property_type::checked>(c_id) == "true"); });
                    detail::register_element(detail::make_element("input", std::format(R"(type="checkbox"{})", !p_initial_value ? std::string_view{} : R"(checked="checked")"sv), std::forward<tp_string_constructible_type_t>(p_id), p_label_text, p_label_size, p_inline_style));
                    return *this;
                }
            };
        }
        auto constexpr checkbox = detail::checkbox_t{};

        namespace detail {
            struct text_box_t {
                template <can_construct<std::string> tp_string_constructible_type1_t, can_construct<std::string> tp_string_constructible_type2_t = std::string_view>
                web_mayunused auto operator()(tp_string_constructible_type1_t&& p_id, tp_string_constructible_type2_t&& p_initial_value = {}, const std::optional<std::string>& p_label_text = {}, const std::optional<std::size_t>& p_label_size = {}, const std::optional<style>& p_inline_style = {}) const -> text_box_t {
                    detail::register_element (detail::make_element("textarea", {}, p_id, p_label_text, p_label_size, p_inline_style, std::forward<tp_string_constructible_type2_t>(p_initial_value)));
                    return *this;
                }
            };
        }
        auto constexpr text_box = detail::text_box_t{};

        namespace detail {
            struct dropdown_t {
                template <can_construct<std::string> tp_string_constructible_type_t, detail::range_of<std::string> tp_string_range_t = std::initializer_list<std::string>>
                web_mayunused auto operator()(tp_string_constructible_type_t&& p_id, tp_string_range_t&& p_options, const std::optional<std::function<void(std::string)>>& p_on_option_change_callable = {}, const std::optional<std::string>& p_label_text = {}, const std::optional<std::size_t>& p_label_size = {}, const std::optional<style>& p_inline_style = {}) const -> dropdown_t{
                    if (p_on_option_change_callable.has_value())
                        detail::register_event_listener(p_id, event_type::change, [c_id = std::string{p_id}, c_on_option_change_callable = p_on_option_change_callable.value()]{ c_on_option_change_callable(get_element_property<element_property_type::value>(c_id)); });
                    detail::register_element(detail::make_element("select", {}, std::forward<tp_string_constructible_type_t>(p_id), p_label_text, p_label_size, p_inline_style, detail::join_strings(std::views::transform(p_options, [&](auto&& a) { return std::format(R"(<option id="{}_{}" value="{}">{}</option>)", p_id, a, a, a); }))));
                    return *this;
                }
            };
        }
        auto constexpr dropdown = detail::dropdown_t{};

        namespace detail {
            struct selection_t {
                template <can_construct<std::string> tp_string_constructible_type_t, detail::range_of<std::string> tp_string_range_t = std::initializer_list<std::string>>
                web_mayunused auto operator()(tp_string_constructible_type_t&& p_id, tp_string_range_t&& p_options, const std::optional<std::function<void(std::string)>>& p_on_option_change_callable = {}, const std::optional<std::string>& p_label_text = {}, const std::optional<std::size_t>& p_label_size = {}, const std::optional<style>& p_inline_style = {}) const -> selection_t {
                    if (p_on_option_change_callable.has_value())
                        detail::register_event_listener(p_id, event_type::change,  [c_id = std::string{p_id}, c_on_option_change_callable = p_on_option_change_callable.value()]{ c_on_option_change_callable(get_element_property<element_property_type::selected_options>(c_id)); });
                    detail::register_element(detail::make_element("select multiple", {}, std::forward<tp_string_constructible_type_t>(p_id), p_label_text, p_label_size, p_inline_style, detail::join_strings(std::views::transform(p_options, [&](auto&& a) { return std::format(R"(<option id="{}_{}" value="{}">{}</option>)", p_id, a, a, a); }))));
                    return *this;
                }
            };
        }
        auto constexpr selection = detail::selection_t{};

        namespace detail {
            struct list_box_t {
                template <can_construct<std::string> tp_string_constructible_type_t, detail::range_of<std::string> tp_string_range_t = std::vector<std::string>>
                web_mayunused auto operator()(tp_string_constructible_type_t&& p_id, bool initialize_to_first_option, const axis_type p_axis_type, tp_string_range_t&& p_options, const std::optional<std::function<void(std::string)>>& p_on_selectable_change_callable = {}, const std::optional<std::string>& p_label_text = {}, const std::optional<std::size_t>& p_label_size = {}, const std::optional<style>& p_inline_style_list_box = {}, const std::optional<style>& p_inline_style_items = {}) const -> list_box_t {
                    if (p_on_selectable_change_callable.has_value())
                        detail::register_event_listener(p_id, event_type::change, [c_id = std::string{p_id}, c_on_selectable_change_callable = p_on_selectable_change_callable.value()]{ c_on_selectable_change_callable(get_selected_list_box_item(c_id)); });
                    detail::register_element(
                        std::format(R"(<style> .list_box{{white-space:nowrap;}}#{} ._list_box_radio_buttons{{display:none;}}#{} ._list_box_radio_buttons+.list_box_item{{display:{};}}</style>)", p_id, p_id, p_axis_type == axis_type::horizontal ? "inline-flex" : "flex") +
                        detail::make_element(
                            "div", R"(class="list_box")", p_id, p_label_text, p_label_size, p_inline_style_list_box,
                            detail::join_strings(std::views::transform(p_options, [&](auto&& a) {
                                auto l_result = std::format(R"(<input type="radio" id="{}_{}" class="_list_box_radio_buttons" name="{}" value="{}"{}><label for="{}_{}" class="list_box_item" {}>{}</label>)", p_id, a, p_id, a, !initialize_to_first_option ? std::string_view{} : R"( checked="checked")"sv, p_id, a, !p_inline_style_items.has_value() ? std::string_view{} : std::string_view{p_inline_style_items.value().to_inline_string()}, a);
                                initialize_to_first_option = false;
                                return l_result;
                            }))
                        )
                    );
                    return *this;
                }
            };
        }
        auto constexpr list_box = detail::list_box_t{};

        //add label, and on item click?
        namespace detail {
            struct table_t {
                template <detail::can_construct<std::string> tp_string_constructible_type_t, detail::range2d_of<std::string> tp_range2d_of_string_t = std::initializer_list<std::initializer_list<std::string>>>
                web_mayunused auto operator()(tp_string_constructible_type_t&& p_id, const bool p_sticky_header, tp_range2d_of_string_t&& p_data, const std::optional<style>& p_inline_style = {}) const -> table_t {
                    bool l_first_row = true;
                    detail::register_element(
                        detail::make_element("div", R"(class="table_wrap")", p_id, {}, {}, p_inline_style,
                            std::format("<style>table{{border-collapse:collapse;width:100%;height:100%;margin:0px}}{}</style>", !p_sticky_header ? "" : "th{position:sticky;top:-1px;}") +
                            detail::make_element("table", {}, p_id, {}, {}, {},
                                detail::join_strings(std::views::transform(p_data, [&](auto&& a) {
                                    auto l_result= std::format("<tr>{}</tr>", detail::join_strings(std::views::transform(a, [&](auto&& b) { return std::format("<t{}>{}</t{}>", l_first_row ? "h" : "d", b, l_first_row ? "h" : "d"); })) );
                                    l_first_row = false;
                                    return l_result;
                                }))
                            )
                        ));
                    return *this;
                }
            };
        }
        auto constexpr table = detail::table_t{};

    //add label
        namespace detail {
            struct collapsing_header_t {
                template <detail::can_construct<std::string> tp_string_constructible_type_t>
                web_mayunused auto operator()(tp_string_constructible_type_t&& p_id, const std::function<void(void)>& p_contents, const std::optional<style>& p_inline_style = {}) const -> collapsing_header_t {
                    auto constexpr static l_no_margins = "margin:0;margin-left:0;margin-top:0;margin-right:0;margin-bottom:0;";
                    auto l_element    = detail::make_element("details", {}, p_id, {}, {}, p_inline_style, std::format(R"(<summary style="{}">{}</summary><div class="collapsing_header_body" style="overflow:auto;{}">[[body]]</div>)", l_no_margins, p_id, l_no_margins));
                    auto l_split_view = std::views::split(l_element, "[[body]]"sv);
                    auto l_it         = std::ranges::begin(l_split_view);
                    detail::register_element(std::ranges::to<std::string>(*l_it));
                    p_contents();
                    detail::register_element(std::ranges::to<std::string>(*std::ranges::next(l_it)));
                    return *this;
                }
            };
        }
        auto constexpr collapsing_header = detail::collapsing_header_t{};

    //add label
        namespace detail {
            template <detail::can_construct<std::string> tp_string_constructible_type_t>
            auto child_window_impl(tp_string_constructible_type_t&& p_id, const overflow_encapsulation p_overflow_encapsulation, const wrap_type p_wrap_type, const std::function<void(void)>& p_contents, const std::string& p_properties, const std::optional<style>& p_inline_style = {}) -> void {
                auto l_element =
                    std::format("<style>#{}{{{}{}{}}}</style>", p_id, p_wrap_type == wrap_type::disabled ? "" : "white-space:nowrap;", p_overflow_encapsulation == overflow_encapsulation::disabled ? "" : "overflow:auto;", p_properties) +
                    detail::make_element("div", R"(class="child_window")", p_id, {}, {}, p_inline_style, "[[body]]");
                auto l_split_view = std::views::split(l_element, "[[body]]"sv);
                auto l_it         = std::ranges::begin(l_split_view);
                detail::register_element(std::ranges::to<std::string>(*l_it));
                p_contents();
                detail::register_element(std::ranges::to<std::string>(*std::next(l_it)));
            }
        };
        namespace detail {
            struct child_window_t {
                template <detail::can_construct<std::string> tp_string_constructible_type_t>
                web_mayunused auto operator()(tp_string_constructible_type_t&& p_id, const overflow_encapsulation p_overflow_encapsulation, const wrap_type p_wrap_type, const std::function<void(void)>& p_contents, const std::optional<style>& p_inline_style = {}) const -> child_window_t {
                    detail::child_window_impl(std::forward<tp_string_constructible_type_t>(p_id), p_overflow_encapsulation, p_wrap_type, p_contents, {}, p_inline_style);
                    return *this;
                }
                template <detail::can_construct<std::string> tp_string_constructible_type_t>
                web_mayunused auto operator()(tp_string_constructible_type_t&& p_id, const overflow_encapsulation p_overflow_encapsulation, const wrap_type p_wrap_type, const flex_display_type p_display_type, const flex_content_justification_type p_justification_type, const std::function<void(void)>& p_contents, const std::optional<style>& p_inline_style = {}) const -> child_window_t {
                    detail::child_window_impl(std::forward<tp_string_constructible_type_t>(p_id), p_overflow_encapsulation, p_wrap_type, p_contents, std::format("display:{};justify-content:{};", detail::flex_display_type_strings[std::to_underlying(p_display_type)], detail::flex_content_justification_type_strings[std::to_underlying(p_justification_type)]), p_inline_style);
                    return *this;
                }
                template <detail::can_construct<std::string> tp_string_constructible_type_t>
                web_mayunused auto operator()(tp_string_constructible_type_t&& p_id, const overflow_encapsulation p_overflow_encapsulation, const wrap_type p_wrap_type, const non_flex_display_type p_display_type, const non_flex_item_justification_type p_justification_type, const std::function<void(void)>& p_contents, const std::optional<style>& p_inline_style = {}) const -> child_window_t {
                    detail::child_window_impl(std::forward<tp_string_constructible_type_t>(p_id), p_overflow_encapsulation, p_wrap_type, p_contents, std::format("display:{};justify-items:{};", detail::non_flex_display_type_strings[std::to_underlying(p_display_type)], detail::non_flex_item_justification_type_strings[std::to_underlying(p_justification_type)]), p_inline_style);
                    return *this;
                }
            };
        }
        auto constexpr child_window = detail::child_window_t{};

    //add label, and on_menu_item_activate_callable
        namespace detail {
            struct menu_t {
                template <detail::can_construct<std::string> tp_string_constructible_type_t, detail::range_of<std::pair<std::string, std::vector<std::string>>> tp_range_of_pair_of_string_and_vector_of_string_t = std::vector<std::pair<std::string, std::vector<std::string>>>>
                web_mayunused auto operator()(tp_string_constructible_type_t&& p_id, tp_range_of_pair_of_string_and_vector_of_string_t&& p_data, const std::optional<style>& p_inline_style = {}) const -> menu_t {
                    detail::register_element(std::format("{}{}",
                        "<style>"
                        "ul{display:inline-block;padding:0;font-size:0;}"
                        "ul li{display:inline-block;height:100%;padding:0px 0px;margin:0px;border:0px;}"
                        "li:focus-within .menu_dropdown{display:block;}"
                        ".menu_dropdown {display:none;position:absolute;padding:0px 0px;margin:0px;}"
                        ".menu_dropdown li{display:block;}"
                        ".menu_dropdown_item{width:100%;text-align:left;margin:0px}"
                        ".menu_dropdown_item:focus{display:none;}"
                        "</style>",
                        detail::make_element("ul", R"(class="menu")", p_id, {}, {}, p_inline_style,
                            detail::join_strings(std::ranges::to<std::vector>(std::views::transform(p_data, [&](auto&& a) {
                                return std::format("<li>{}</li>",
                                    std::format(R"(<button id="{}" class="menu_item" style="height:100%">{}</button>)", std::format("{}_{}", p_id, a.first), a.first) +
                                    std::format(R"(<ul class="menu_dropdown">{}</ul>)", detail::join_strings(std::views::transform(a.second, [&](auto&& b) { return std::format(R"(<li><button id="{}" class="menu_dropdown_item">{}</button></li>)", std::format("{}_{}_{}", p_id, a.first, b), b); })))
                                );
                            })))
                        )
                    ));
                    return *this;
                }
            };
        }
        auto constexpr menu = detail::menu_t{};

    //add on drag callback, enable/disable splitter?
    //remove relativity_type? relativity_type::absolute doesn't work with this properly, not sure what i was trying to do with the relativity_type
        namespace detail {
            struct window_splitter_t {
                web_mayunused auto operator()(std::string p_id, const axis_type p_axis_type, std::string p_window_id_1, std::string p_window_id_2, const relativity_type p_relativity, const std::size_t p_thickness_size, const std::optional<style>& p_inline_style = {}) const -> window_splitter_t {
                    auto l_axis   = p_axis_type == axis_type::horizontal ? "horizontal" : "vertical";
                    auto l_cursor = p_axis_type == axis_type::horizontal ? "ns-resize" : "ew-resize";
                    detail::register_element(
                        std::format("<style>.window_splitter_{}, .window_splitter_{}:hover{{cursor:{};position:absolute;margin:0px;padding:0px;width:0px;height:0px;border:0px;background-color:transparent;}} .window_splitter_{}:active{{cursor:{};}}</style>", l_axis, l_axis, l_cursor, l_axis, l_cursor) +
                        detail::make_element("div", std::format(R"(class="window_splitter_{}")", l_axis), p_id, {}, {}, p_inline_style)
                    );
                    detail::register_script([c_id = std::move(p_id), c_axis_type = p_axis_type, c_window_id_1 = std::move(p_window_id_1), c_window_id_2 = std::move(p_window_id_2), c_relativity = p_relativity, c_thickness_size = p_thickness_size] {
                        EM_ASM((
                            var splitter     = document.getElementById(UTF8ToString($0));
                            var windowA      = document.getElementById(UTF8ToString($1));
                            var windowB      = document.getElementById(UTF8ToString($2));
                            var container    = windowA.parentElement;
                            var isRelative   = $3;
                            var isHorizontal = $4;
                            var thickness    = $5;

                            var isDragging = false;
                            var startMouse, beforeA, beforeB;
                            var containerSize = isHorizontal ? container.offsetWidth : container.offsetHeight;

                            function ResizeSplitter(event) {
                                windowARect             = windowA.getBoundingClientRect();
                                scrollX                 = window.scrollX || window.pageXOffset;
                                scrollY                 = window.scrollY || window.pageYOffset;
                                splitter.style.left     = (isHorizontal ? (windowARect.left + windowARect.width + scrollX - (thickness / 2)) : windowARect.left) + 'px';
                                splitter.style.top      = (isHorizontal? (windowARect.top + scrollY) : (windowARect.top + windowARect.height + scrollY - (thickness / 2))) + 'px';
                                splitter.style.width    = (isHorizontal ? thickness : windowARect.width) + 'px';
                                splitter.style.height   = (isHorizontal ? windowARect.height : thickness) + 'px';
                                splitter.style.position = 'absolute';
                                splitter.style.zIndex   = '9999';
                            }
                            splitter.addEventListener("mousedown", function (e) {
                                isDragging = true;
                                startMouse = isHorizontal ? e.pageX : e.pageY;
                                beforeA = isRelative ? Math.round((isHorizontal ? windowA.offsetWidth : windowA.offsetHeight) / containerSize * 100) : (isHorizontal ? windowA.offsetWidth : windowA.offsetHeight);
                                beforeB = isRelative ? Math.round((isHorizontal ? windowB.offsetWidth : windowB.offsetHeight) / containerSize * 100) : (isHorizontal ? windowB.offsetWidth : windowB.offsetHeight);
                            });
                            document.addEventListener("mousemove", function (e) {
                                if (!isDragging)
                                    return;
                                dragAmount = (isHorizontal ? e.pageX : e.pageY) - startMouse;
                                dragAmountRelative = dragAmount / containerSize * 100;
                                afterA = beforeA + Math.round(isRelative ? (dragAmount / containerSize * 100) : dragAmount);
                                afterB = beforeB - Math.round(isRelative ? (dragAmount / containerSize * 100) : dragAmount);
                                if ((dragAmountRelative >= 1 || dragAmountRelative <= -1) && afterA >= (containerSize * 0.001) && afterB >= (containerSize * 0.001)) {
                                    if (isHorizontal) {
                                        windowA.style.width = afterA + (isRelative ? "%" : "px");
                                        windowB.style.width = afterB + (isRelative ? "%" : "px");
                                    }
                                    else {
                                        windowA.style.height = afterA + (isRelative ? "%" : "px");
                                        windowB.style.height = afterB + (isRelative ? "%" : "px");
                                    }
                                }
                            });
                            document.addEventListener("mouseup", function () {
                                isDragging = false;
                            });
                            window.addEventListener("resize", function () {
                                containerSize = (isHorizontal ? container.offsetWidth : container.offsetHeight);
                            });
                            window.addEventListener("mouseover", ResizeSplitter);
                            window.addEventListener("change", ResizeSplitter);
                            window.addEventListener('resize', ResizeSplitter);
                        ), c_id.c_str(), std::ranges::data(c_window_id_1), std::ranges::data(c_window_id_2), c_relativity == relativity_type::relative, c_axis_type == axis_type::vertical, c_thickness_size);
                    });
                    return *this;
                }
            };
        }
        auto constexpr window_splitter = detail::window_splitter_t{};
    }
};

web_nodiscard auto operator""_abs(unsigned long long p_value) web_noexcept(true) -> web::integral_absolute {
    return web::integral_absolute{p_value};
}
web_nodiscard auto operator""_rel(unsigned long long p_value) web_noexcept(true) -> web::integral_relative {
    return web::integral_relative{p_value};
}
web_nodiscard auto operator""_cal(const char* p_value, const std::size_t p_size) web_noexcept(true) -> web::calculation_string {
    return web::calculation_string{p_value};
}
