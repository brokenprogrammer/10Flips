using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace _10FlipServer.Models
{
    public class Card
    {
        public CardType Type;
        public int Value
        {
            get
            {
                return ((int) (Type - 1) % 13) + 2;
            }
        }
        public int Suit
        {
            get
            {
                return ((int) Type - 1) / 13;
            }
        }

        public Card(CardType type)
        {
            this.Type = type;
        }
    }

    public enum CardType
    {
        CARD_TYPE_NULL = 0,
        TWO_OF_HEARTS = 1,
        THREE_OF_HEARTS,
        FOUR_OF_HEARTS,
        FIVE_OF_HEARTS,
        SIX_OF_HEARTS,
        SEVEN_OF_HEARTS,
        EIGHT_OF_HEARTS,
        NINE_OF_HEARTS,
        TEN_OF_HEARTS,
        JACK_OF_HEARTS,
        QUEEN_OF_HEARTS,
        KING_OF_HEARTS,
        ACE_OF_HEARTS,

        TWO_OF_CLUBS,
        THREE_OF_CLUBS,
        FOUR_OF_CLUBS,
        FIVE_OF_CLUBS,
        SIX_OF_CLUBS,
        SEVEN_OF_CLUBS,
        EIGHT_OF_CLUBS,
        NINE_OF_CLUBS,
        TEN_OF_CLUBS,
        JACK_OF_CLUBS,
        QUEEN_OF_CLUBS,
        KING_OF_CLUBS,
        ACE_OF_CLUBS,

        TWO_OF_DIAMONDS,
        THREE_OF_DIAMONDS,
        FOUR_OF_DIAMONDS,
        FIVE_OF_DIAMONDS,
        SIX_OF_DIAMONDS,
        SEVEN_OF_DIAMONDS,
        EIGHT_OF_DIAMONDS,
        NINE_OF_DIAMONDS,
        TEN_OF_DIAMONDS,
        JACK_OF_DIAMONDS,
        QUEEN_OF_DIAMONDS,
        KING_OF_DIAMONDS,
        ACE_OF_DIAMONDS,

        TWO_OF_SPADES,
        THREE_OF_SPADES,
        FOUR_OF_SPADES,
        FIVE_OF_SPADES,
        SIX_OF_SPADES,
        SEVEN_OF_SPADES,
        EIGHT_OF_SPADES,
        NINE_OF_SPADES,
        TEN_OF_SPADES,
        JACK_OF_SPADES,
        QUEEN_OF_SPADES,
        KING_OF_SPADES,
        ACE_OF_SPADES,

        CARD_TYPE_FLIPPED,
    };
}
