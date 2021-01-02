#include "mm.hpp"

bool items_init () {
    uint32_t iter;
    for (item *_item = (item *) s.items.iter_init (true, &iter); _item;
            _item = (item *) s.items.iterate ()) {
        item_activate (_item, "SELF", "SELF");
    }
    s.items.iter_init (false, &iter);
    return true;
}

bool items_tick (float t) {
    uint32_t iter;
    for (item *_item = (item *) s.items.iter_init (true, &iter); _item;
            _item = (item *) s.items.iterate ()) {
        item_move (_item, t);

        // animation
        if (_item->_obj->anim_) {
            _item->_obj->anim_t += t;

            // anim overrun
            if (_item->_obj->anim_t >= _item->_obj->anim_total)
                _item->_obj->anim_t -= _item->_obj->anim_total;

            _item->_obj->anim ();
        }

        if ((_item->_item_state_cur) && (! _item->move_stalled_)) {
            if (_item->_item_state_cur->len != 0.0f) {
                _item->_item_state_cur->tick += 30.0f;
                if (_item->_item_state_cur->tick >
                        _item->_item_state_cur->len) {
                    _item->_item_state_cur->tick = 0.0f;
                    _item->state_c = _item->_item_state_cur->next;
                    _item->_item_state_cur =
                        (item_state *) _item->states.get (_item->state_c);
                    item_activate (_item, "SELF", "SELF");
                }
            }
        }
    }
    s.items.iter_init (false, &iter);
    return true;
}

bool item_activate (item *_item, const char *source_type,
        const char *source_s) {
    item *target_item = NULL;
    item_action *_item_action;
    bool state_change_ = false;
    uint32_t iter;

    dlog::debug ("item_activate", _item->name_s, source_type, source_s);

    if (! _item->_item_state_cur)
        return false;

    for (_item_action = (item_action *)
            _item->_item_state_cur->actions.iter_init (true, &iter);
            _item_action; _item_action = (item_action *)
                _item->_item_state_cur->actions.iterate ()) {
        if (strcmp (_item_action->source_type, "ALL"))
            if (strcmp (source_type, _item_action->source_type))
                continue;
        if (strcmp (_item_action->source_s, "ALL"))
            if (strcmp (source_s, _item_action->source_s))
                continue;

        // find target
        if (_item_action->target_s) {
            if (! strcmp (_item_action->target_s, "SELF"))
                target_item = _item;
            else {
                target_item = item_get (_item_action->target_s, NULL);
                if (! target_item) {
                    _item->_item_state_cur->actions.iter_init (false, &iter);
                    return false;
                }
            }
        }

        switch (_item_action->action_id) {
            case ITEM_ACT_WIN:
                s.map_win_ = true;
                break;

            case ITEM_ACT_PLAYER_JUMP:
                // FIXME arbitrary t
                player_jump (0.01f);
                break;

            case ITEM_ACT_DESTROY:
                if (! target_item)
                    break;

                if (target_item == _item) {
                    _item->_item_state_cur->actions.iter_init (false, &iter);
                    item_del (target_item->name_s);
                    return true;
                }
                else 
                    item_del (target_item->name_s);
                break;

            case ITEM_ACT_SET_STATE:
                dlog::debug ("set_state", target_item->name_s,
                    _item_action->data1);

                if (target_item->_item_state_cur)
                    target_item->_item_state_cur->tick = 0.0f;

                if (! _item_action->data2)
                    target_item->state_c = _item_action->data1;
                else
                    target_item->state_c += _item_action->data1;

                if (target_item == _item)
                    state_change_ = true;
                else {
                    target_item->_item_state_cur = (item_state *)
                        target_item->states.get (target_item->state_c);
                    item_activate (target_item, "SELF", "SELF");
                }
                break;

            case ITEM_ACT_MOVE:
                if (_item_action->data1) {
                    math::v_copy (target_item->move_v, _item_action->v);
                    target_item->move_len = _item_action->fdata1;
                }
                else {
                    math::v_sub2 (target_item->move_v, _item_action->v,
                        target_item->_ent->pos);
                    target_item->move_len = _item_action->fdata1;
                }
                break;

            case ITEM_ACT_PLAYER_MOVE:
                if (_item_action->data1)
                    s._app->phys.move (s._player_ent, _item_action->v);
                else
                    math::v_copy (s._player_ent->pos, _item_action->v);
                break;

            case ITEM_ACT_SOUND:
                _item_action->_snd->play (false);
                _item_action->_snd->pos3d (_item->_ent->pos);
                break;

            case ITEM_ACT_ANIM_INIT:
                target_item->_obj->anim_init (_item_action->data1,
                    _item_action->data2);
                break;

            case ITEM_ACT_ANIM_SEQ:
                target_item->_obj->anim_set_seq (_item_action->data1);
                break;

            case ITEM_ACT_ANIM_PLAY:
                target_item->_obj->anim_ = true;
                break;

            case ITEM_ACT_ANIM_PAUSE:
                target_item->_obj->anim_ = false;
                break;
        }
    }
    _item->_item_state_cur->actions.iter_init (false, &iter);

    if (state_change_) {
        _item->_item_state_cur = (item_state *) _item->states.get
            (_item->state_c);
        item_activate (_item, "SELF", "SELF");
    }

    return true;
}

item *item_load (char *file_s, char *name_s, float *pos) {
    char b_in [PATH_MAX];
    char kw [160], ops [6][160];
    FILE *f_in;
    item *_item;
    item_state *_item_state = NULL;
    item_action *_item_action = NULL;
    char *source_type = NULL, *source_s = NULL;
    
    dlog::debug ("item_load", file_s, name_s);

    f_in = fopen (file_s, "r");
    if (! f_in)
        return NULL;

    _item = new item ();
    _item->type = NULL;
    _item->file_s = strdup (file_s);
    _item->name_s = strdup (name_s);
    _item->move_len = 0.0f;
    _item->move_tick = 0.0f;
    _item->move_stalled_ = false;
    _item->_obj = NULL;
    _item->state_c = 0;
    _item->_item_state_cur = NULL;

    _item->_ent = s._app->phys.ent_add ();
    if (! _item->_ent)
        return NULL;
    _item->_ent->type = s.TYPE_ITEM;
    _item->_ent->parent = _item;
    math::v_copy (_item->_ent->pos, pos);
    _item->_ent->cell = s._app->cell.classify (_item->_ent->pos);

    while (fgets (b_in, sizeof(b_in), f_in)) {
        if (b_in [strlen (b_in) - 1] == '\n')
            b_in[strlen (b_in) - 1] = 0;
        if (! b_in[0])
            continue;
        else if (b_in[0] == ';')
            continue;

        n1_var::var_start (b_in);
        if (! n1_var::var_next (kw))
            continue;
        if (! strcmp (kw, "type")) {
            if (! n1_var::var_next (ops[0]))
                continue;
            _item->type = strdup (ops[0]);
        }
        else if (! strcmp (kw, "weight")) {
            if (! n1_var::var_next (ops[0]))
                continue;    
            _item->_ent->weight = (float) atof (ops[0]);
        }
        else if (! strcmp (kw, "bound_only")) {
            _item->_ent->bound_only = true;
        }
        else if (! strcmp (kw, "object")) {
            if (! n1_var::var_next (ops[0]))
                continue;
            _item->_obj = new n1_obj (s._app);
            if (! _item->_obj->load (ops[0]))
                return NULL;
            _item->_ent->_obj = _item->_obj;
        }
        else if (! strcmp (kw, "state")) {
            _item_state = new item_state ();
            _item_state->len = 0.0f;
            _item_state->tick = 0.0f;
            if (! _item->states.add (_item_state))
                return NULL;
            _item_state->next = _item->states.item_n;
        }
        else if (! strcmp (kw, "len")) {
            if (! n1_var::var_next (ops[0]))
                continue;
            _item_state->len = (float) atof (ops[0]);
        }
        else if (! strcmp (kw, "next")) {
            if (! n1_var::var_next (ops[0]))
                continue;
            _item_state->next = atoi (ops[0]);
        }
        else if (! strcmp (kw, "source")) {
            if (! n1_var::var_next (ops[0]))
                continue;
            if (! n1_var::var_next (ops[1]))
                continue;

            if (source_type)
                free (source_type);
            if (source_s)
                free (source_s);

            source_type = strdup (ops[0]);
            source_s = strdup (ops[1]);
        }
        else if (! strcmp (kw, "win")) {
            _item_action = _item_state->action_add (source_type, source_s);
            if (! _item_action)
                return NULL;
            _item_action->action_id = ITEM_ACT_WIN;
        }
        else if (! strcmp (kw, "jump")) {
            _item_action = _item_state->action_add (source_type, source_s);
            if (! _item_action)
                return NULL;
            _item_action->action_id = ITEM_ACT_PLAYER_JUMP;
        }
        else if (! strcmp (kw, "destroy")) {
            if (! n1_var::var_next (ops[0]))
                continue;

            _item_action = _item_state->action_add (source_type, source_s);
            if (! _item_action)
                return NULL;
            _item_action->action_id = ITEM_ACT_DESTROY;
            _item_action->target_s = strdup (ops[0]);
        }
        else if (! strcmp (kw, "set_state")) {
            if (! n1_var::var_next (ops[0]))
                continue;
            if (! n1_var::var_next (ops[1]))
                continue;
            if (! n1_var::var_next (ops[2]))
                continue;

            _item_action = _item_state->action_add (source_type, source_s);
            if (! _item_action)
                return NULL;
            _item_action->action_id = ITEM_ACT_SET_STATE;
            _item_action->target_s = strdup (ops[0]);
            _item_action->data1 = atoi (ops[1]);
            _item_action->data2 = atoi (ops[2]);
        }
        else if (! strcmp (kw, "move")) {
            if (! n1_var::var_next (ops[0]))
                continue;
            if (! n1_var::var_next (ops[1]))
                continue;
            if (! n1_var::var_next (ops[2]))
                continue;
            if (! n1_var::var_next (ops[3]))
                continue;
            if (! n1_var::var_next (ops[4]))
                continue;
            if (! n1_var::var_next (ops[5]))
                continue;

            _item_action = _item_state->action_add (source_type, source_s);
            if (! _item_action)
                return NULL;
            _item_action->action_id = ITEM_ACT_MOVE;

            _item_action->target_s = strdup (ops[0]);
            _item_action->v[0] = (float) atof (ops[1]);
            _item_action->v[1] = (float) atof (ops[2]);
            _item_action->v[2] = (float) atof (ops[3]);
            _item_action->data1 = atoi (ops[4]);
            _item_action->fdata1 = (float) atof (ops[5]);
        }
        else if (! strcmp (kw, "move_player")) {
            if (! n1_var::var_next (ops[0]))
                continue;
            if (! n1_var::var_next (ops[1]))
                continue;
            if (! n1_var::var_next (ops[2]))
                continue;
            if (! n1_var::var_next (ops[3]))
                continue;

            _item_action = _item_state->action_add (source_type, source_s);
            if (! _item_action)
                return NULL;
            _item_action->action_id = ITEM_ACT_PLAYER_MOVE;
            _item_action->v[0] = (float) atof (ops[0]);
            _item_action->v[1] = (float) atof (ops[1]);
            _item_action->v[2] = (float) atof (ops[2]);
            _item_action->data1 = atoi (ops[3]);
        }
        else if (! strcmp (kw, "sound")) {
            if (! n1_var::var_next (ops[0]))
                continue;

            _item_action = _item_state->action_add (source_type, source_s);
            if (! _item_action)
                return NULL;
            _item_action->action_id = ITEM_ACT_SOUND;
            _item_action->_snd = new snd (ops[0]);
        }
        else if (! strcmp (kw, "anim_init")) {
            if (! n1_var::var_next (ops[0]))
                continue;
            if (! n1_var::var_next (ops[1]))
                continue;
            if (! n1_var::var_next (ops[2]))
                continue;

            _item_action = _item_state->action_add (source_type, source_s);
            if (! _item_action)
                return NULL;
            _item_action->action_id = ITEM_ACT_ANIM_INIT;
            _item_action->target_s = strdup (ops[0]);
            _item_action->data1 = atoi (ops[1]);
            _item_action->data2 = atoi (ops[2]);
        }
        else if (! strcmp (kw, "anim_seq")) {
            if (! n1_var::var_next (ops[0]))
                continue;
            if (! n1_var::var_next (ops[1]))
                continue;

            _item_action = _item_state->action_add (source_type, source_s);
            if (! _item_action)
                return NULL;
            _item_action->action_id = ITEM_ACT_ANIM_SEQ;
            _item_action->target_s = strdup (ops[0]);
            _item_action->data1 = atoi (ops[1]);
        }
        else if (! strcmp (kw, "anim_play")) {
            if (! n1_var::var_next (ops[0]))
                continue;
            if (! n1_var::var_next (ops[1]))
                continue;

            _item_action = _item_state->action_add (source_type, source_s);
            if (! _item_action)
                return NULL;
            _item_action->action_id = ITEM_ACT_ANIM_PLAY;
            _item_action->target_s = strdup (ops[0]);
            _item_action->data1 = atoi (ops[1]);
        }
        else if (! strcmp (kw, "anim_pause")) {
            if (! n1_var::var_next (ops[0]))
                continue;

            _item_action = _item_state->action_add (source_type, source_s);
            if (! _item_action)
                return NULL;
            _item_action->action_id = ITEM_ACT_ANIM_PAUSE;
            _item_action->target_s = strdup (ops[0]);
        }
    }

    fclose (f_in);

    _item->_item_state_cur = (item_state *) _item->states.get (_item->state_c);
            
    if (source_type)
        free (source_type);
    if (source_s)
        free (source_s);

    s.items.add (_item);
    return _item;
}

bool item_move (item *_item, float t) {
    float dir [3];
    n1_phys::ecol col;
    bool res_;

    if (_item->move_len == 0.0f)
        return true;

    _item->move_tick += t;
    if (_item->move_tick > _item->move_len) {
        t = _item->move_len - _item->move_tick;
        _item->move_tick = _item->move_len;
    }

    // TODO: check for precision errors - seems iffy
    math::v_scalef2 (dir, _item->move_v, t);

    if (_item == s.player_pon_item) {
        res_ = s._app->phys.move (s._player_ent, dir);
        // TODO
    }

    // FIXME 
    res_ = s._app->phys.collide_all (_item->_ent->pos, _item->_ent->_obj->brad,
        dir, &col, _item->_ent);
    if (res_) {
        _item->move_stalled_ = true;

        // cellmap collision
        if (col.hit_type == n1_phys::TYPE_CELL)
            item_activate (_item, "map", "cell");
        // item collision
        else if (col.hit_type == s.TYPE_ITEM) {
            item_activate ((item *) col.hit_parent, _item->type, _item->name_s);
        }

        return true;
    }

    _item->move_stalled_ = false;
    math::v_add (_item->_ent->pos, dir);
    _item->_ent->cell = s._app->cell.classify2 (_item->_ent->pos,
        _item->_ent->cell);

    if (_item->move_tick == _item->move_len)
        _item->move_len = 0.0f;

    return true;
}

item *item_get (char *name_s, char *file_s) {
    item *_item;
    uint32_t iter;

    if (name_s) {
        for (_item = (item *) s.items.iter_init (true, &iter); _item;
                _item = (item *) s.items.iterate ())
            if (! strcmp (_item->name_s, name_s)) {
                s.items.iter_init (false, &iter);
                return _item;
            }
        s.items.iter_init (false, &iter);
    }
    else if (file_s) {
        for (_item = (item *) s.items.iter_init (true, &iter); _item;
                _item = (item *) s.items.iterate ())
            if (! strcmp (_item->file_s, file_s)) {
                s.items.iter_init (false, &iter);
                return _item;
            }
        s.items.iter_init (false, &iter);
    }
    return NULL;
}

bool item_del (item *_item) {
    if (s.player_pon_item == _item)
        s.player_pon_item = NULL;

    s.items.del (_item, 0, item::destroy);
    return true;
}

bool item_del (char *name_s) {
    item *_item;

    _item = item_get (name_s, NULL);
    if (! _item)
        return false;
    
    return item_del (_item);
}

void item::destroy (void *pv) {
    item *_item = (item *) pv;
    if (! _item)
        return;

    if (_item->type)
        free (_item->type);
    if (_item->name_s)
        free (_item->name_s);
    if (_item->file_s)
        free (_item->file_s);

    if (_item->_obj)
        _item->_obj->destroy ();
 
    s._app->phys.ents.del (_item->_ent, -1, blaze::hook_free);

    _item->states.destroy (item_state::destroy);
    free (_item);
}
        
item_action *item_state::action_add (const char *source_type,
        const char *source_s) {
    item_action *_item_action;

    _item_action = (item_action *) malloc (sizeof (item_action));
    if (! _item_action)
        return NULL;
    memset (_item_action, 0, sizeof (item_action));
    _item_action->source_type = strdup (source_type);
    if (! _item_action->source_type)
        return NULL;
    _item_action->source_s = strdup (source_s);
    if (! _item_action->source_s)
        return NULL;

    if (! this->actions.add (_item_action))
        return NULL;
    return _item_action;
}
 
