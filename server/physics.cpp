//
// Created by silvman on 09.05.17.
//

#include "headers/physics.hpp"


// const b2Vec2 ball_default_speed(0.0f, -9.0f);
const b2Vec2 player_default_speed(20.0f, 0.0f);


// blocksKickListener
void physics_scene::blocksKickListener::EndContact(b2Contact* contact) {
    physic_body* bodyUserData_A = reinterpret_cast<physic_body *>(contact->GetFixtureA()->GetBody()->GetUserData());
    physic_body* bodyUserData_B = reinterpret_cast<physic_body *>(contact->GetFixtureB()->GetBody()->GetUserData());

    if ( bodyUserData_A->getId() == block_id ) {
        reinterpret_cast<physic_block *>(bodyUserData_A)->setKicked();
    }

    if ( bodyUserData_B->getId() == block_id ) {
        reinterpret_cast<physic_block *>(bodyUserData_B)->setKicked();
    }

    if ( bodyUserData_A->getId() == player_bottom_id ) {
        reinterpret_cast<physic_player *>(bodyUserData_A)->setKicked();
        reinterpret_cast<physic_ball *>(bodyUserData_B)->setOwner(player_bottom_id);
        return;
    }

    if ( bodyUserData_A->getId() == player_top_id ) {
        reinterpret_cast<physic_player *>(bodyUserData_A)->setKicked();
        reinterpret_cast<physic_ball *>(bodyUserData_B)->setOwner(player_top_id);
        return;
    }

    if ( bodyUserData_B->getId() == player_bottom_id ) {
        reinterpret_cast<physic_player *>(bodyUserData_B)->setKicked();
        reinterpret_cast<physic_ball *>(bodyUserData_A)->setOwner(player_bottom_id);
        return;
    }

    if ( bodyUserData_B->getId() == player_top_id ) {
        reinterpret_cast<physic_player *>(bodyUserData_B)->setKicked();
        reinterpret_cast<physic_ball *>(bodyUserData_A)->setOwner(player_top_id);
        return;
    }
}



// physic_body
physics_scene::physic_body::physic_body(b2World& world, object_type id)
        : world(world), id(id) { }

physics_scene::physic_body::~physic_body() {}

object_type physics_scene::physic_body::getId() const {
    return id;
}



// physic_ball
physics_scene::physic_ball::physic_ball(b2World& world, const float play_pos_x, const float play_pos_y)
        : physic_body(world, ball_id)
{
    // последний параметр определяет где находится шарик - у верхнего (true) или у нижнего (false) игрока
    ball = createBall(play_pos_x, play_pos_y, false);
}

physics_scene::physic_ball::~physic_ball() { world.DestroyBody(ball); }

b2Body* physics_scene::physic_ball::createBall(const float play_pos_x, const float play_pos_y, const bool is_top) {
    b2Body* new_ball;

    b2BodyDef ball_def;
    ball_def.type = b2_dynamicBody;

    if(is_top) {
        owner = player_top_id;
        ball_def.position.Set(play_pos_x, play_pos_y + 5 / PTM); // тут еще остается ptm TODO
    } else {
        owner = player_bottom_id;
        ball_def.position.Set(play_pos_x, play_pos_y - 15 / PTM); // тут еще остается ptm
    }

    new_ball = world.CreateBody(&ball_def);

    b2CircleShape ball_shape;
    ball_shape.m_radius = 10/PTM;

    b2FixtureDef ball_fixture_def;
    ball_fixture_def.shape = &ball_shape;
    ball_fixture_def.density = 10.1f;
    ball_fixture_def.restitution = 1.01f;
    ball_fixture_def.friction = 0.0f;

    // из-за особенностей хранения userData
    new_ball->CreateFixture(&ball_fixture_def);

    new_ball->SetUserData(this);

    new_ball->SetBullet(true);

    return new_ball;
};

void physics_scene::physic_ball::restart(const b2Vec2& player_position, const bool is_top) {
    world.DestroyBody(ball);
    is_launched = false;

    ball = createBall(player_position.x, player_position.y, is_top);
}

void physics_scene::physic_ball::lauch(const num_action push, const players pl) {
    is_launched = true;

    if (pl == bottom) {
            switch (push) {
                case push_vertical: {
                    ball->SetLinearVelocity(b2Vec2(0.0f, -9.0f));
                    break;
                }

                case push_left: {
                    ball->SetLinearVelocity(b2Vec2(-4.0f, -8.0f));
                    break;
                }

                case push_right: {
                    ball->SetLinearVelocity(b2Vec2(4.0f, -8.0f));
                    break;
                }

                case push_righter: {
                    ball->SetLinearVelocity(b2Vec2(5.0f, -5.0f));
                    break;
                }

                case push_lefter: {
                    ball->SetLinearVelocity(b2Vec2(-5.0f, -5.0f));
                    break;
                }

                default: {
                    break;
                }
            }
    }

    if (pl == top) {
            switch (push) {
                case push_vertical: {
                    ball->SetLinearVelocity(b2Vec2(0.0f, 9.0f));
                    break;
                }

                case push_left: {
                    ball->SetLinearVelocity(b2Vec2(-4.0f, 8.0f));
                    break;
                }

                case push_right: {
                    ball->SetLinearVelocity(b2Vec2(4.0f, 8.0f));
                    break;
                }

                case push_righter: {
                    ball->SetLinearVelocity(b2Vec2(5.0f, 5.0f));
                    break;
                }

                case push_lefter: {
                    ball->SetLinearVelocity(b2Vec2(-5.0f, 5.0f));
                    break;
                }

                default: {
                    break;
                }
            }
    }
}

void physics_scene::physic_ball::move_with_player(const b2Vec2& speed, const num_move dest) {
    switch (dest) {
        case move_right: {
            ball->SetLinearVelocity(speed);
            break;
        }

        case move_left: {
            ball->SetLinearVelocity(-speed);
            break;
        }

            // дописал default, ибо не по православному без него
        case stay:
        default: {
            ball->SetLinearVelocity(b2Vec2(0,0));
            break;
        }
    }
}

void physics_scene::physic_ball::setOwner(object_type type) {
    owner = type;
}

object_type physics_scene::physic_ball::getOwner() const {
    return owner;
}

const b2Vec2& physics_scene::physic_ball::giveCoords() const {
    return ball->GetPosition();
}

const b2Vec2 physics_scene::physic_ball::giveSpeed() const {
    return ball->GetLinearVelocity();
}

bool physics_scene::physic_ball::isLaunched() const {
    return is_launched;
}



// physic_player
physics_scene::physic_player::physic_player(b2World& world, const float start_x, const float start_y, const object_type id)
        : physic_body(world, id), player_speed(player_default_speed)
{
    b2BodyDef player_def;
    player_def.type = b2_kinematicBody;
    player_def.position.Set((start_x - 10) / PTM, (start_y - 10) / PTM);
    player = world.CreateBody(&player_def);

    b2PolygonShape player_shape;
    player_shape.SetAsBox(50.0f/PTM, 5.0f/PTM);

    b2FixtureDef player_fixture_def;
    player_fixture_def.shape = &player_shape;
    player_fixture_def.density = 10.1f;
    player_fixture_def.restitution = 1;
    player_fixture_def.friction = 0.0f;

    player->CreateFixture(&player_fixture_def);
    player->SetUserData(this);
}

physics_scene::physic_player::~physic_player() { world.DestroyBody(player); }

void physics_scene::physic_player::stop() {
    player->SetLinearVelocity(b2Vec2(0,0));
}

void physics_scene::physic_player::move(const num_move dest) {
    float x_player = player->GetPosition().x;

    switch (dest) {
        case move_right: {
            player->SetLinearVelocity(player_speed);

            if(x_player > (gc_window_size_x - 80) / PTM) {
                stop();
            }

            break;
        }

        case move_left: {
            player->SetLinearVelocity(-player_speed);

            if(x_player < 70 / PTM) {
                stop();
            }
            break;
        }

        default:
        case stay: {
            stop();
            break;
        }
    }
}

bool physics_scene::physic_player::checkKicked() {
    if(isKicked) {
        isKicked = false;
        return true;
    }

    return false;
}

const b2Vec2 physics_scene::physic_player::getSpeed() const {
    return player->GetLinearVelocity();
}

void physics_scene::physic_player::setKicked() {
    isKicked = true;
}

const b2Vec2& physics_scene::physic_player::giveCoords() const {
    return player->GetPosition();
}



// physic_border
physics_scene::physic_border::physic_border(b2World& world, const float pos_x, const float pos_y, const float size_x, const float size_y)
        : physic_body(world, border_id)
{
    b2BodyDef border_def;
    b2PolygonShape border_shape;

    border_def.position.Set((pos_x - 10)/PTM, (pos_y - 10)/PTM);
    border_shape.SetAsBox(size_x/PTM, size_y/PTM);
    border = world.CreateBody(&border_def);
    border->CreateFixture(&border_shape, 0.0f);

    border->SetUserData(this);
}

physics_scene::physic_border::~physic_border() { world.DestroyBody(border); }



// physic_block
physics_scene::physic_block::physic_block(
        b2World& world,
        const float pos_x, const float pos_y,
        const float size_x, const float size_y,
        const float angle, const int number
) : physic_body(world, block_id), is_kicked(false), number(number)
{
    b2BodyDef block_def;
    b2PolygonShape block_shape;

    block_def.position.Set((pos_x)/PTM, (pos_y)/PTM);
    block_def.angle = angle / DEG;
    block_shape.SetAsBox(size_x / 2 / PTM, size_y / 2 / PTM);
    block = world.CreateBody(&block_def);
    block->CreateFixture(&block_shape, 0.0f);

    block->SetUserData(this);
}

physics_scene::physic_block::~physic_block() { world.DestroyBody(block); }

int physics_scene::physic_block::try_kick() {
    if(is_kicked) {
        block->SetActive(0);
        is_kicked = false;
        return number;
    }

    return -1;
}

void physics_scene::physic_block::setKicked() {
    is_kicked = true;
}



// physics_scene
physics_scene::physics_scene(const float window_size_x, const float window_size_y)
        : world(b2Vec2(0.0f, 0.0f)),
          ball(world, (window_size_x/ 2), (window_size_y - 35)),
          player_bottom(world, (window_size_x / 2), (window_size_y - 35), player_bottom_id),
          player_top(world, (window_size_x / 2), 35, player_top_id),
          right_border(world, (window_size_x - 10), (window_size_y / 2), 10.0f, window_size_y / 2),
          left_border(world, 0.0f, (window_size_y / 2), 10.0f, window_size_y / 2),
          broken_block(-1),
          sound_player(false),
          who_broke_the_block(undef)
{
    world.SetContactListener(&listener);

    // нужно для правильного индексирования блоков, считает число ячеек с нулём
    int passaway = 0;

    for(int i = 0; i < map_rows; i++) {
        for(int j = 0; j < map_cols; j++){
            if (map_blocks[i][j]) {
                blocks.push_back(
                        new physic_block(
                                world,
                                map_start_x + j * map_width + j * map_space,
                                map_start_y + i * map_height + i * map_space,
                                map_width,
                                map_height,
                                0.0f,
                                (i * map_cols) + j - passaway
                        )
                );
            } else {
                passaway++;
            }
        }
    }
}

physics_scene::~physics_scene() {
    for(auto it = blocks.begin(); it != blocks.end(); it++) {
        delete(*it);
    }
}

void physics_scene::analyseKeys(physic_player& player, const players who_leads_the_ball, const num_move move, const num_action action) {
    switch (move) {
        case move_left:
        case move_right: {
            player.move(move);
            break;
        }

        default:
        case stay: {
            player.stop();
            break;
        }
    }

    if( !ball.isLaunched() ) {
        switch (action) {
            case push_vertical:
            case push_right:
            case push_righter:
            case push_left:
            case push_lefter: {
                ball.lauch(action, who_leads_the_ball);
                break;
            }

            default:
                break;
        }
    }
}

void physics_scene::moveBall(physic_player& player, const num_move move) {
    switch (move) {
        case move_left: {
            if( !ball.isLaunched() )
                ball.move_with_player(-player.getSpeed(), move);
            break;
        }

        case move_right: {
            if( !ball.isLaunched() )
                ball.move_with_player(player.getSpeed(), move);
            break;
        }

        default:
        case stay: {
            if( !ball.isLaunched() )
                ball.move_with_player(player.getSpeed(), stay);
            break;
        }
    }
}

void physics_scene::calculate(
        const num_move key_bottom_move, const num_action key_bottom_action,
        const num_move key_top_move, const num_action key_top_action,
        const players who_lost_the_ball, const players who_leads_the_ball
) {

    dt = clock.getElapsedTime().asSeconds();
    clock.restart();

    broken_block = -1;
    who_broke_the_block = undef;

    switch (who_lost_the_ball) {
        case bottom: {
            // первый параметр передает координаты игрока, потерявшего шарик
            // второй параметр - является ли положение верхним
            ball.restart(player_bottom.giveCoords(), false);
            break;
        }

        case top: {
            ball.restart(player_top.giveCoords(), true);
            break;
        }

        default: break;
    }

    analyseKeys(player_bottom, who_leads_the_ball, key_bottom_move, key_bottom_action);
    analyseKeys(player_top, who_leads_the_ball, key_top_move, key_top_action);

    if (who_leads_the_ball == bottom) {
        moveBall(player_bottom, key_bottom_move);
    } else {
        moveBall(player_top, key_top_move);
    }

    int buf;
    for (auto it = blocks.begin(); it != blocks.end(); it++) {
        buf = (*it)->try_kick();
        if (buf != -1) {
            broken_block = buf;
            who_broke_the_block = ball.getOwner();
            break;
        }
    }

    world.Step(dt, velocityIterations, positionIterations);


    //std::cout << dt << std::endl;
}

bool physics_scene::checkPlayerKicked() {
    return player_top.checkKicked() || player_bottom.checkKicked();
}

const sf::Vector2f physics_scene::givePlayerBottomCoords() const {
    b2Vec2 coords = player_bottom.giveCoords();
    return sf::Vector2f(coords.x * PTM, coords.y * PTM);
}

const sf::Vector2f physics_scene::givePlayerTopCoords() const {
    b2Vec2 coords = player_top.giveCoords();
    return sf::Vector2f(coords.x * PTM, coords.y * PTM);
}

const sf::Vector2f physics_scene::giveBallCoords() const {
    b2Vec2 coords = ball.giveCoords();
    return sf::Vector2f(coords.x * PTM, coords.y * PTM - 10);
}

const sf::Vector2f physics_scene::giveBallSpeed() const {
    b2Vec2 coords = ball.giveSpeed();
    return sf::Vector2f(coords.x * PTM, coords.y * PTM);
}
/*
const sf::Vector2f givePlayerBottomSpeed() const {
    b2Vec2 coords = player_bottom.getSpeed();
    return sf::Vector2f(coords.x * PTM, coords.y * PTM);

}
const sf::Vector2f givePlayerTopSpeed() const {
    b2Vec2 coords = player_top.getSpeed();
    return sf::Vector2f(coords.x * PTM, coords.y * PTM);

}
*/
const int physics_scene::getBrokenBlock() const {
    return broken_block;
}

players physics_scene::getHitman() const {
    switch (who_broke_the_block) {
        case player_bottom_id: {
            return bottom;
        }

        case player_top_id: {
            return top;
        }

        default: {
            return no;
        }
    }
}